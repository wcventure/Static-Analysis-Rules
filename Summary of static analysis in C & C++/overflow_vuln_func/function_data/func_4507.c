static int mxf_parse_structural_metadata(MXFContext *mxf)
{
    MXFPackage *material_package = NULL;
    MXFPackage *temp_package = NULL;
    int i, j, k;

    av_dlog(mxf->fc, "metadata sets count %d\n", mxf->metadata_sets_count);
    /
    for (i = 0; i < mxf->packages_count; i++) {
        material_package = mxf_resolve_strong_ref(mxf, &mxf->packages_refs[i], MaterialPackage);
        if (material_package) break;
    }
    if (!material_package) {
        av_log(mxf->fc, AV_LOG_ERROR, "no material package found\n");
        return AVERROR_INVALIDDATA;
    }

    for (i = 0; i < material_package->tracks_count; i++) {
        MXFPackage *source_package = NULL;
        MXFTrack *material_track = NULL;
        MXFTrack *source_track = NULL;
        MXFTrack *temp_track = NULL;
        MXFDescriptor *descriptor = NULL;
        MXFStructuralComponent *component = NULL;
        UID *essence_container_ul = NULL;
        const MXFCodecUL *codec_ul = NULL;
        const MXFCodecUL *container_ul = NULL;
        AVStream *st;

        if (!(material_track = mxf_resolve_strong_ref(mxf, &material_package->tracks_refs[i], Track))) {
            av_log(mxf->fc, AV_LOG_ERROR, "could not resolve material track strong ref\n");
            continue;
        }

        if (!(material_track->sequence = mxf_resolve_strong_ref(mxf, &material_track->sequence_ref, Sequence))) {
            av_log(mxf->fc, AV_LOG_ERROR, "could not resolve material track sequence strong ref\n");
            continue;
        }

        /
        for (j = 0; j < material_track->sequence->structural_components_count; j++) {
            /
            component = mxf_resolve_strong_ref(mxf, &material_track->sequence->structural_components_refs[j], SourceClip);
            if (!component)
                continue;

            for (k = 0; k < mxf->packages_count; k++) {
                temp_package = mxf_resolve_strong_ref(mxf, &mxf->packages_refs[k], SourcePackage);
                if (!temp_package)
                    continue;
                if (!memcmp(temp_package->package_uid, component->source_package_uid, 16)) {
                    source_package = temp_package;
                    break;
                }
            }
            if (!source_package) {
                av_log(mxf->fc, AV_LOG_ERROR, "material track %d: no corresponding source package found\n", material_track->track_id);
                break;
            }
            for (k = 0; k < source_package->tracks_count; k++) {
                if (!(temp_track = mxf_resolve_strong_ref(mxf, &source_package->tracks_refs[k], Track))) {
                    av_log(mxf->fc, AV_LOG_ERROR, "could not resolve source track strong ref\n");
                    return AVERROR_INVALIDDATA;
                }
                if (temp_track->track_id == component->source_track_id) {
                    source_track = temp_track;
                    break;
                }
            }
            if (!source_track) {
                av_log(mxf->fc, AV_LOG_ERROR, "material track %d: no corresponding source track found\n", material_track->track_id);
                break;
            }
        }
        if (!source_track)
            continue;

        st = avformat_new_stream(mxf->fc, NULL);
        if (!st) {
            av_log(mxf->fc, AV_LOG_ERROR, "could not allocate stream\n");
            return AVERROR(ENOMEM);
        }
        st->id = source_track->track_id;
        st->priv_data = source_track;
        st->duration = component->duration;
        if (st->duration == -1)
            st->duration = AV_NOPTS_VALUE;
        st->start_time = component->start_position;
        avpriv_set_pts_info(st, 64, material_track->edit_rate.num, material_track->edit_rate.den);

        if (!(source_track->sequence = mxf_resolve_strong_ref(mxf, &source_track->sequence_ref, Sequence))) {
            av_log(mxf->fc, AV_LOG_ERROR, "could not resolve source track sequence strong ref\n");
            return AVERROR_INVALIDDATA;
        }

        PRINT_KEY(mxf->fc, "data definition   ul", source_track->sequence->data_definition_ul);
        codec_ul = mxf_get_codec_ul(ff_mxf_data_definition_uls, &source_track->sequence->data_definition_ul);
        st->codec->codec_type = codec_ul->id;

        source_package->descriptor = mxf_resolve_strong_ref(mxf, &source_package->descriptor_ref, AnyType);
        if (source_package->descriptor) {
            if (source_package->descriptor->type == MultipleDescriptor) {
                for (j = 0; j < source_package->descriptor->sub_descriptors_count; j++) {
                    MXFDescriptor *sub_descriptor = mxf_resolve_strong_ref(mxf, &source_package->descriptor->sub_descriptors_refs[j], Descriptor);

                    if (!sub_descriptor) {
                        av_log(mxf->fc, AV_LOG_ERROR, "could not resolve sub descriptor strong ref\n");
                        continue;
                    }
                    if (sub_descriptor->linked_track_id == source_track->track_id) {
                        descriptor = sub_descriptor;
                        break;
                    }
                }
            } else if (source_package->descriptor->type == Descriptor)
                descriptor = source_package->descriptor;
        }
        if (!descriptor) {
            av_log(mxf->fc, AV_LOG_INFO, "source track %d: stream %d, no descriptor found\n", source_track->track_id, st->index);
            continue;
        }
        PRINT_KEY(mxf->fc, "essence codec     ul", descriptor->essence_codec_ul);
        PRINT_KEY(mxf->fc, "essence container ul", descriptor->essence_container_ul);
        essence_container_ul = &descriptor->essence_container_ul;
        /
        if (IS_KLV_KEY(essence_container_ul, mxf_encrypted_essence_container)) {
            av_log(mxf->fc, AV_LOG_INFO, "broken encrypted mxf file\n");
            for (k = 0; k < mxf->metadata_sets_count; k++) {
                MXFMetadataSet *metadata = mxf->metadata_sets[k];
                if (metadata->type == CryptoContext) {
                    essence_container_ul = &((MXFCryptoContext *)metadata)->source_container_ul;
                    break;
                }
            }
        }
        /
        codec_ul = mxf_get_codec_ul(ff_mxf_codec_uls, &descriptor->essence_codec_ul);
        st->codec->codec_id = codec_ul->id;
        if (descriptor->extradata) {
            st->codec->extradata = descriptor->extradata;
            st->codec->extradata_size = descriptor->extradata_size;
        }
        if (st->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            container_ul = mxf_get_codec_ul(mxf_essence_container_uls, essence_container_ul);
            if (st->codec->codec_id == CODEC_ID_NONE)
                st->codec->codec_id = container_ul->id;
            st->codec->width = descriptor->width;
            st->codec->height = descriptor->height;
            if (st->codec->codec_id == CODEC_ID_RAWVIDEO)
                st->codec->pix_fmt = descriptor->pix_fmt;
            st->need_parsing = AVSTREAM_PARSE_HEADERS;
        } else if (st->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            container_ul = mxf_get_codec_ul(mxf_essence_container_uls, essence_container_ul);
            if (st->codec->codec_id == CODEC_ID_NONE)
                st->codec->codec_id = container_ul->id;
            st->codec->channels = descriptor->channels;
            st->codec->bits_per_coded_sample = descriptor->bits_per_sample;
            st->codec->sample_rate = descriptor->sample_rate.num / descriptor->sample_rate.den;
            /
            if (st->codec->codec_id == CODEC_ID_PCM_S16LE) {
                if (descriptor->bits_per_sample > 16 && descriptor->bits_per_sample <= 24)
                    st->codec->codec_id = CODEC_ID_PCM_S24LE;
                else if (descriptor->bits_per_sample == 32)
                    st->codec->codec_id = CODEC_ID_PCM_S32LE;
            } else if (st->codec->codec_id == CODEC_ID_PCM_S16BE) {
                if (descriptor->bits_per_sample > 16 && descriptor->bits_per_sample <= 24)
                    st->codec->codec_id = CODEC_ID_PCM_S24BE;
                else if (descriptor->bits_per_sample == 32)
                    st->codec->codec_id = CODEC_ID_PCM_S32BE;
            } else if (st->codec->codec_id == CODEC_ID_MP2) {
                st->need_parsing = AVSTREAM_PARSE_FULL;
            }
        }
        if (st->codec->codec_type != AVMEDIA_TYPE_DATA && (*essence_container_ul)[15] > 0x01) {
            av_log(mxf->fc, AV_LOG_WARNING, "only frame wrapped mappings are correctly supported\n");
            st->need_parsing = AVSTREAM_PARSE_FULL;
        }
    }
    return 0;
}
