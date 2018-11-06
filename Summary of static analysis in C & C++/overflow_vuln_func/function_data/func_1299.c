static void
dissect_mq_pdu(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	proto_tree	*mq_tree = NULL;
	proto_tree	*mqroot_tree = NULL;
	proto_item	*ti = NULL;
	gint offset = 0;
	guint32 structId = MQ_STRUCTID_NULL;
	guint8 opcode;
	guint32 iSegmentLength = 0;
	guint32 iSizePayload = 0;
	gint iSizeMD = 0;
	gboolean bLittleEndian = FALSE;
	gboolean bPayload = FALSE;
	gboolean bEBCDIC = FALSE;
	gint iDistributionListSize = 0;
	struct mq_msg_properties tMsgProps;
	static gint iPreviousFrameNumber = -1;

	if (check_col(pinfo->cinfo, COL_PROTOCOL)) col_set_str(pinfo->cinfo, COL_PROTOCOL, "MQ");
	if (check_col(pinfo->cinfo, COL_INFO))
	{
		/
		if (iPreviousFrameNumber != (gint) pinfo->fd->num)
			col_clear(pinfo->cinfo, COL_INFO);
		else
			col_append_str(pinfo->cinfo, COL_INFO, " | ");
	}
	iPreviousFrameNumber = pinfo->fd->num;
	if (tvb_length(tvb) >= 4)
	{
		structId = tvb_get_ntohl(tvb, offset);
		if ((structId == MQ_STRUCTID_TSH || structId == MQ_STRUCTID_TSH_EBCDIC) && tvb_length_remaining(tvb, offset) >= 28)
		{
			/
			gint iSizeTSH = 28;
			guint8 iControlFlags = 0;
			if (structId == MQ_STRUCTID_TSH_EBCDIC) bEBCDIC = TRUE;
			opcode = tvb_get_guint8(tvb, offset + 9);
			bLittleEndian = (tvb_get_guint8(tvb, offset + 8) == MQ_LITTLE_ENDIAN ? TRUE : FALSE);
			iSegmentLength = tvb_get_ntohl(tvb, offset + 4);
			iControlFlags = tvb_get_guint8(tvb, offset + 10);

			if (check_col(pinfo->cinfo, COL_INFO))
			{
				col_append_fstr(pinfo->cinfo, COL_INFO, "%s", val_to_str(opcode, mq_opcode_vals, "Unknown (0x%02x)"));
			}

			if (tree)
			{
				ti = proto_tree_add_item(tree, proto_mq, tvb, offset, -1, FALSE);
				proto_item_append_text(ti, " (%s)", val_to_str(opcode, mq_opcode_vals, "Unknown (0x%02x)"));
				if (bEBCDIC == TRUE) proto_item_append_text(ti, " (EBCDIC)");
				mqroot_tree = proto_item_add_subtree(ti, ett_mq);

				ti = proto_tree_add_text(mqroot_tree, tvb, offset, iSizeTSH, MQ_TEXT_TSH);
				mq_tree = proto_item_add_subtree(ti, ett_mq_tsh);

				proto_tree_add_item(mq_tree, hf_mq_tsh_structid, tvb, offset + 0, 4, FALSE);
				proto_tree_add_item(mq_tree, hf_mq_tsh_packetlength, tvb, offset + 4, 4, FALSE);

				proto_tree_add_item(mq_tree, hf_mq_tsh_byteorder, tvb, offset + 8, 1, FALSE);

				proto_tree_add_item(mq_tree, hf_mq_tsh_opcode, tvb, offset + 9, 1, FALSE);

				/
				{
					proto_tree	*mq_tree_sub = NULL;

					ti = proto_tree_add_item(mq_tree, hf_mq_tsh_controlflags, tvb, offset + 10, 1, FALSE);
					mq_tree_sub = proto_item_add_subtree(ti, ett_mq_tsh_tcf);

					proto_tree_add_boolean(mq_tree_sub, hf_mq_tsh_tcf_dlq, tvb, offset + 10, 1, iControlFlags);
					proto_tree_add_boolean(mq_tree_sub, hf_mq_tsh_tcf_reqacc, tvb, offset + 10, 1, iControlFlags);
					proto_tree_add_boolean(mq_tree_sub, hf_mq_tsh_tcf_last, tvb, offset + 10, 1, iControlFlags);
					proto_tree_add_boolean(mq_tree_sub, hf_mq_tsh_tcf_first, tvb, offset + 10, 1, iControlFlags);
					proto_tree_add_boolean(mq_tree_sub, hf_mq_tsh_tcf_closechann, tvb, offset + 10, 1, iControlFlags);
					proto_tree_add_boolean(mq_tree_sub, hf_mq_tsh_tcf_reqclose, tvb, offset + 10, 1, iControlFlags);
					proto_tree_add_boolean(mq_tree_sub, hf_mq_tsh_tcf_error, tvb, offset + 10, 1, iControlFlags);
					proto_tree_add_boolean(mq_tree_sub, hf_mq_tsh_tcf_confirmreq, tvb, offset + 10, 1, iControlFlags);
				}

				proto_tree_add_item(mq_tree, hf_mq_tsh_reserved, tvb, offset + 11, 1, FALSE);
				proto_tree_add_item(mq_tree, hf_mq_tsh_luwid, tvb, offset + 12, 8, FALSE);
				proto_tree_add_item(mq_tree, hf_mq_tsh_encoding, tvb, offset + 20, 4, bLittleEndian);
				proto_tree_add_item(mq_tree, hf_mq_tsh_ccsid, tvb, offset + 24, 2, bLittleEndian);
				proto_tree_add_item(mq_tree, hf_mq_tsh_padding, tvb, offset + 26, 2, FALSE);
			}
			offset += iSizeTSH;

			/
			if (tvb_length_remaining(tvb, offset) >= 4)
			{
				structId = tvb_get_ntohl(tvb, offset);
				if (((iControlFlags & MQ_TCF_FIRST) != 0) || opcode < 0x80)
				{
					/
					gint iSizeAPI = 16;
					if (opcode >= 0x80 && opcode <= 0x9F && tvb_length_remaining(tvb, offset) >= 16)
					{
						guint32 iReturnCode = 0;
						iReturnCode = tvb_get_guint32_endian(tvb, offset + 8, bLittleEndian);
						if (check_col(pinfo->cinfo, COL_INFO))
						{
							if (iReturnCode != 0)
								col_append_fstr(pinfo->cinfo, COL_INFO, " [RC=%d]", iReturnCode);
						}

						if (tree)
						{
							ti = proto_tree_add_text(mqroot_tree, tvb, offset, iSizeAPI, MQ_TEXT_API);
							mq_tree = proto_item_add_subtree(ti, ett_mq_api);

							proto_tree_add_item(mq_tree, hf_mq_api_replylength, tvb, offset, 4, FALSE);
							proto_tree_add_item(mq_tree, hf_mq_api_completioncode, tvb, offset + 4, 4, bLittleEndian);
							proto_tree_add_item(mq_tree, hf_mq_api_reasoncode, tvb, offset + 8, 4, bLittleEndian);
							proto_tree_add_item(mq_tree, hf_mq_api_objecthandle, tvb, offset + 12, 4, bLittleEndian);
						}
						offset += iSizeAPI;
						structId = (tvb_length_remaining(tvb, offset) >= 4) ? tvb_get_ntohl(tvb, offset) : MQ_STRUCTID_NULL;
					}
					if ((structId == MQ_STRUCTID_MSH || structId == MQ_STRUCTID_MSH_EBCDIC) && tvb_length_remaining(tvb, offset) >= 20)
					{
						gint iSizeMSH = 20;
						iSizePayload = tvb_get_guint32_endian(tvb, offset + 16, bLittleEndian);
						bPayload = TRUE;
						if (tree)
						{
							ti = proto_tree_add_text(mqroot_tree, tvb, offset, iSizeMSH, MQ_TEXT_MSH);
							mq_tree = proto_item_add_subtree(ti, ett_mq_msh);

							proto_tree_add_item(mq_tree, hf_mq_msh_structid, tvb, offset + 0, 4, FALSE);
							proto_tree_add_item(mq_tree, hf_mq_msh_seqnum, tvb, offset + 4, 4, bLittleEndian);
							proto_tree_add_item(mq_tree, hf_mq_msh_datalength, tvb, offset + 8, 4, bLittleEndian);
							proto_tree_add_item(mq_tree, hf_mq_msh_unknown1, tvb, offset + 12, 4, bLittleEndian);
							proto_tree_add_item(mq_tree, hf_mq_msh_msglength, tvb, offset + 16, 4, bLittleEndian);
						}
						offset += iSizeMSH;
					}
					else if (opcode == MQ_TST_STATUS && tvb_length_remaining(tvb, offset) >= 8)
					{
						/
						guint32 iStatus = 0;
						gint iStatusLength = 0;
						iStatus = tvb_get_guint32_endian(tvb, offset + 4, bLittleEndian);
						iStatusLength = tvb_get_guint32_endian(tvb, offset, bLittleEndian);

						if (tvb_length_remaining(tvb, offset) >= iStatusLength)
						{
							if (check_col(pinfo->cinfo, COL_INFO))
							{
								if (iStatus != 0)
									col_append_fstr(pinfo->cinfo, COL_INFO, ": Code=%s", val_to_str(iStatus, mq_status_vals, "Unknown (0x%08x)"));
							}
							if (tree)
							{
								ti = proto_tree_add_text(mqroot_tree, tvb, offset, 8, MQ_TEXT_STAT);
								mq_tree = proto_item_add_subtree(ti, ett_mq_status);

								proto_tree_add_item(mq_tree, hf_mq_status_length, tvb, offset, 4, bLittleEndian);
								proto_tree_add_item(mq_tree, hf_mq_status_code, tvb, offset + 4, 4, bLittleEndian);

								if (iStatusLength >= 12)
									proto_tree_add_item(mq_tree, hf_mq_status_value, tvb, offset + 8, 4, bLittleEndian);
							}
							offset += iStatusLength;
						}
					}
					else if (opcode == MQ_TST_PING && tvb_length_remaining(tvb, offset) > 4)
					{
						if (tree)
						{
							ti = proto_tree_add_text(mqroot_tree, tvb, offset, -1, MQ_TEXT_PING);
							mq_tree = proto_item_add_subtree(ti, ett_mq_ping);

							proto_tree_add_item(mq_tree, hf_mq_ping_length, tvb, offset, 4, bLittleEndian);
							proto_tree_add_item(mq_tree, hf_mq_ping_buffer, tvb, offset + 4, -1, FALSE);
						}
						offset = tvb_length(tvb);
					}
					else if (opcode == MQ_TST_RESET && tvb_length_remaining(tvb, offset) >= 8)
					{
						if (tree)
						{
							ti = proto_tree_add_text(mqroot_tree, tvb, offset, -1, MQ_TEXT_RESET);
							mq_tree = proto_item_add_subtree(ti, ett_mq_reset);

							proto_tree_add_item(mq_tree, hf_mq_reset_length, tvb, offset, 4, bLittleEndian);
							proto_tree_add_item(mq_tree, hf_mq_reset_seqnum, tvb, offset + 4, 4, bLittleEndian);
						}
						offset = tvb_length(tvb);
					}
					else if (opcode == MQ_TST_MQCONN && tvb_length_remaining(tvb, offset) > 0)
					{
						gint iSizeCONN = 0;
						/ /
						/
						/
						iSizeCONN = iSegmentLength - iSizeTSH - iSizeAPI;
						if (iSizeCONN != 112 && iSizeCONN != 120) iSizeCONN = 0;

						if (iSizeCONN != 0 && tvb_length_remaining(tvb, offset) >= iSizeCONN)
						{
							if (check_col(pinfo->cinfo, COL_INFO))
							{
								guint8* sApplicationName;
								guint8* sQueueManager;
								sApplicationName = tvb_get_ephemeral_string(tvb, offset + 48, 28);
								if (strip_trailing_blanks(sApplicationName, 28) != 0)
								{
									col_append_fstr(pinfo->cinfo, COL_INFO, ": App=%s", sApplicationName);
								}
								sQueueManager = tvb_get_ephemeral_string(tvb, offset, 48);
								if (strip_trailing_blanks(sQueueManager, 48) != 0)
								{
									col_append_fstr(pinfo->cinfo, COL_INFO, " QM=%s", sQueueManager);
								}
							}


							if (tree)
							{
								ti = proto_tree_add_text(mqroot_tree, tvb, offset, iSizeCONN, MQ_TEXT_CONN);
								mq_tree = proto_item_add_subtree(ti, ett_mq_conn);

								proto_tree_add_item(mq_tree, hf_mq_conn_queuemanager, tvb, offset, 48, FALSE);
								proto_tree_add_item(mq_tree, hf_mq_conn_appname, tvb, offset + 48, 28, FALSE);
								proto_tree_add_item(mq_tree, hf_mq_conn_apptype, tvb, offset + 76, 4, bLittleEndian);
								proto_tree_add_item(mq_tree, hf_mq_conn_acttoken, tvb, offset + 80, 32, FALSE);

								if (iSizeCONN >= 120)
								{
									proto_tree_add_item(mq_tree, hf_mq_conn_version, tvb, offset + 112, 4, bLittleEndian);
									proto_tree_add_item(mq_tree, hf_mq_conn_options, tvb, offset + 116, 4, bLittleEndian);
								}
							}
							offset += iSizeCONN;
						}
					}
					else if ((opcode == MQ_TST_MQINQ || opcode == MQ_TST_MQINQ_REPLY || opcode == MQ_TST_MQSET) && tvb_length_remaining(tvb, offset) >= 12)
					{
						/
						gint iNbSelectors = 0;
						gint iNbIntegers = 0;
						gint iCharLen = 0;
						gint iOffsetINQ = 0;
						gint iSelector = 0;

						iNbSelectors = tvb_get_guint32_endian(tvb, offset, bLittleEndian);
						iNbIntegers = tvb_get_guint32_endian(tvb, offset + 4, bLittleEndian);
						iCharLen = tvb_get_guint32_endian(tvb, offset + 8, bLittleEndian);

						if (tree)
						{
							ti = proto_tree_add_text(mqroot_tree, tvb, offset, -1, MQ_TEXT_INQ);
							mq_tree = proto_item_add_subtree(ti, ett_mq_inq);

							proto_tree_add_item(mq_tree, hf_mq_inq_nbsel, tvb, offset, 4, bLittleEndian);
							proto_tree_add_item(mq_tree, hf_mq_inq_nbint, tvb, offset + 4, 4, bLittleEndian);
							proto_tree_add_item(mq_tree, hf_mq_inq_charlen, tvb, offset + 8, 4, bLittleEndian);
						}
						iOffsetINQ = 12;
						if (tvb_length_remaining(tvb, offset + iOffsetINQ) >= iNbSelectors * 4)
						{
							if (tree)
							{
								for (iSelector = 0; iSelector < iNbSelectors; iSelector++)
								{
									proto_tree_add_item(mq_tree, hf_mq_inq_sel, tvb, offset + iOffsetINQ + iSelector * 4, 4, bLittleEndian);
								}
							}
							iOffsetINQ += iNbSelectors * 4;
							if (opcode == MQ_TST_MQINQ_REPLY || opcode == MQ_TST_MQSET)
							{
								gint iSizeINQValues = 0;
								iSizeINQValues = iNbIntegers * 4 + iCharLen;
								if (tvb_length_remaining(tvb, offset + iOffsetINQ) >= iSizeINQValues)
								{
									gint iInteger = 0;
									if (tree)
									{
										for (iInteger = 0; iInteger < iNbIntegers; iInteger++)
										{
											proto_tree_add_item(mq_tree, hf_mq_inq_intvalue, tvb, offset + iOffsetINQ + iInteger * 4, 4, bLittleEndian);
										}
									}
									iOffsetINQ += iNbIntegers * 4;
									if (iCharLen != 0)
									{
										if (tree)
										{
											proto_tree_add_item(mq_tree, hf_mq_inq_charvalues, tvb, offset + iOffsetINQ, iCharLen, FALSE);
										}
									}
								}
							}
						}
						offset += tvb_length(tvb);
					}
					else if ((opcode == MQ_TST_SPI || opcode == MQ_TST_SPI_REPLY) && tvb_length_remaining(tvb, offset) >= 12)
					{
						gint iOffsetSPI = 0;
						guint32 iSpiVerb = 0;

						iSpiVerb = tvb_get_guint32_endian(tvb, offset, bLittleEndian);
						if (check_col(pinfo->cinfo, COL_INFO))
						{
							col_append_fstr(pinfo->cinfo, COL_INFO, " (%s)", val_to_str(iSpiVerb, mq_spi_verbs_vals, "Unknown (0x%08x)"));
						}

						if (tree)
						{
							ti = proto_tree_add_text(mqroot_tree, tvb, offset, 12, MQ_TEXT_SPI);
							mq_tree = proto_item_add_subtree(ti, ett_mq_spi);

							proto_tree_add_item(mq_tree, hf_mq_spi_verb, tvb, offset, 4, bLittleEndian);
							proto_tree_add_item(mq_tree, hf_mq_spi_version, tvb, offset + 4, 4, bLittleEndian);
							proto_tree_add_item(mq_tree, hf_mq_spi_length, tvb, offset + 8, 4, bLittleEndian);
						}

						offset += 12;
						structId = (tvb_length_remaining(tvb, offset) >= 4) ? tvb_get_ntohl(tvb, offset) : MQ_STRUCTID_NULL;
						if ((structId == MQ_STRUCTID_SPQU || structId == MQ_STRUCTID_SPAU_EBCDIC
							|| structId == MQ_STRUCTID_SPPU || structId == MQ_STRUCTID_SPPU_EBCDIC
							|| structId == MQ_STRUCTID_SPGU || structId == MQ_STRUCTID_SPGU_EBCDIC
							|| structId == MQ_STRUCTID_SPAU || structId == MQ_STRUCTID_SPAU_EBCDIC)
							&& tvb_length_remaining(tvb, offset) >= 12)
						{
							gint iSizeSPIMD = 0;
							if (tree)
							{
								guint8* sStructId;
								sStructId = tvb_get_ephemeral_string(tvb, offset, 4);
								ti = proto_tree_add_text(mqroot_tree, tvb, offset, 12, "%s", sStructId);
								mq_tree = proto_item_add_subtree(ti, ett_mq_spi_base);

								proto_tree_add_item(mq_tree, hf_mq_spi_base_structid, tvb, offset, 4, FALSE);
								proto_tree_add_item(mq_tree, hf_mq_spi_base_version, tvb, offset + 4, 4, bLittleEndian);
								proto_tree_add_item(mq_tree, hf_mq_spi_base_length, tvb, offset + 8, 4, bLittleEndian);
							}
							offset += 12;
							structId = (tvb_length_remaining(tvb, offset) >= 4) ? tvb_get_ntohl(tvb, offset) : MQ_STRUCTID_NULL;

							if ((iSizeSPIMD = dissect_mq_md(tvb, mqroot_tree, bLittleEndian, offset, &tMsgProps)) != 0)
							{
								gint iSizeGMO = 0;
								gint iSizePMO = 0;
								offset += iSizeSPIMD;

								if ((iSizeGMO = dissect_mq_gmo(tvb, pinfo, mqroot_tree, bLittleEndian, offset)) != 0)
								{
									offset += iSizeGMO;
								}
								else if ((iSizePMO = dissect_mq_pmo(tvb, pinfo, mqroot_tree, bLittleEndian, offset, &iDistributionListSize)) != 0)
								{
									offset += iSizePMO;
								}
								structId = (tvb_length_remaining(tvb, offset) >= 4) ? tvb_get_ntohl(tvb, offset) : MQ_STRUCTID_NULL;
							}

							if ((structId == MQ_STRUCTID_SPQO || structId == MQ_STRUCTID_SPQO_EBCDIC
								|| structId == MQ_STRUCTID_SPQI || structId == MQ_STRUCTID_SPQI_EBCDIC
								|| structId == MQ_STRUCTID_SPPO || structId == MQ_STRUCTID_SPPO_EBCDIC
								|| structId == MQ_STRUCTID_SPPI || structId == MQ_STRUCTID_SPPI_EBCDIC
								|| structId == MQ_STRUCTID_SPGO || structId == MQ_STRUCTID_SPGO_EBCDIC
								|| structId == MQ_STRUCTID_SPGI || structId == MQ_STRUCTID_SPGI_EBCDIC
								|| structId == MQ_STRUCTID_SPAO || structId == MQ_STRUCTID_SPAO_EBCDIC
								|| structId == MQ_STRUCTID_SPAI || structId == MQ_STRUCTID_SPAI_EBCDIC)
								&& tvb_length_remaining(tvb, offset) >= 12)
							{
								if (tree)
								{
									/
									guint8* sStructId;
									sStructId = tvb_get_ephemeral_string(tvb, offset, 4);
									ti = proto_tree_add_text(mqroot_tree, tvb, offset, -1, "%s", sStructId);
									mq_tree = proto_item_add_subtree(ti, ett_mq_spi_base);

									proto_tree_add_item(mq_tree, hf_mq_spi_base_structid, tvb, offset, 4, FALSE);
									proto_tree_add_item(mq_tree, hf_mq_spi_base_version, tvb, offset + 4, 4, bLittleEndian);
									proto_tree_add_item(mq_tree, hf_mq_spi_base_length, tvb, offset + 8, 4, bLittleEndian);
								}

								if (structId == MQ_STRUCTID_SPQO && tvb_length_remaining(tvb, offset) >= 16)
								{
									if (tree)
									{
										gint iVerbNumber = 0;
										proto_tree_add_item(mq_tree, hf_mq_spi_spqo_nbverb, tvb, offset + 12, 4, bLittleEndian);
										iVerbNumber = tvb_get_guint32_endian(tvb, offset + 12, bLittleEndian);

										if (tvb_length_remaining(tvb, offset) >= iVerbNumber * 20 + 16)
										{
											gint iVerb = 0;
											iOffsetSPI = offset + 16;
											for (iVerb = 0; iVerb < iVerbNumber; iVerb++)
											{
												proto_tree_add_item(mq_tree, hf_mq_spi_spqo_verbid, tvb, iOffsetSPI, 4, bLittleEndian);
												proto_tree_add_item(mq_tree, hf_mq_spi_spqo_maxinoutversion, tvb, iOffsetSPI + 4, 4, bLittleEndian);
												proto_tree_add_item(mq_tree, hf_mq_spi_spqo_maxinversion, tvb, iOffsetSPI + 8, 4, bLittleEndian);
												proto_tree_add_item(mq_tree, hf_mq_spi_spqo_maxoutversion, tvb, iOffsetSPI + 12, 4, bLittleEndian);
												proto_tree_add_item(mq_tree, hf_mq_spi_spqo_flags, tvb, iOffsetSPI + 16, 4, bLittleEndian);
												iOffsetSPI += 20;
											}
											offset += iVerbNumber * 20 + 16;
										}
									}
								}
								else if (structId == MQ_STRUCTID_SPAI && tvb_length_remaining(tvb, offset) >= 136)
								{
									if (tree)
									{
										proto_tree_add_item(mq_tree, hf_mq_spi_spai_mode, tvb, offset + 12, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_spi_spai_unknown1, tvb, offset + 16, 48, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_spi_spai_unknown2, tvb, offset + 64, 48, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_spi_spai_msgid, tvb, offset + 112, 24, bLittleEndian);
									}
									offset += 136;
								}
								else if (structId == MQ_STRUCTID_SPGI && tvb_length_remaining(tvb, offset) >= 24)
								{
									if (tree)
									{
										proto_tree_add_item(mq_tree, hf_mq_spi_spgi_batchsize, tvb, offset + 12, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_spi_spgi_batchint, tvb, offset + 16, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_spi_spgi_maxmsgsize, tvb, offset + 20, 4, bLittleEndian);
									}
									offset += 24;
								}
								else if ((structId == MQ_STRUCTID_SPGO || structId == MQ_STRUCTID_SPPI) && tvb_length_remaining(tvb, offset) >= 20)
								{
									if (tree)
									{
										/
										{
											proto_tree	*mq_tree_sub = NULL;
											gint iOptionsFlags;

											ti = proto_tree_add_item(mq_tree, hf_mq_spi_spgo_options, tvb, offset + 12, 4, bLittleEndian);
											mq_tree_sub = proto_item_add_subtree(ti, ett_mq_spi_options);
											iOptionsFlags = tvb_get_guint32_endian(tvb, offset + 12, bLittleEndian);

											proto_tree_add_boolean(mq_tree_sub, hf_mq_spi_options_deferred, tvb, offset + 12, 4, iOptionsFlags);
											proto_tree_add_boolean(mq_tree_sub, hf_mq_spi_options_syncpoint, tvb, offset + 12, 4, iOptionsFlags);
											proto_tree_add_boolean(mq_tree_sub, hf_mq_spi_options_blank, tvb, offset + 12, 4, iOptionsFlags);
										}
										proto_tree_add_item(mq_tree, hf_mq_spi_spgo_size, tvb, offset + 16, 4, bLittleEndian);
									}
									iSizePayload = tvb_get_guint32_endian(tvb, offset + 16, bLittleEndian);
									offset += 20;
									bPayload = TRUE;
								}
								else
								{
									offset += 12;
								}
								structId = (tvb_length_remaining(tvb, offset) >= 4) ? tvb_get_ntohl(tvb, offset) : MQ_STRUCTID_NULL;
							}
						}
					}
					else if ((opcode >= 0xA0 && opcode <= 0xB9) && tvb_length_remaining(tvb, offset) >= 16)
					{
						/
						if (tree)
						{
							ti = proto_tree_add_text(mqroot_tree, tvb, offset, 16, "%s (%s)", MQ_TEXT_XA, val_to_str(opcode, mq_opcode_vals, "Unknown (0x%02x)"));
							mq_tree = proto_item_add_subtree(ti, ett_mq_xa);

							proto_tree_add_item(mq_tree, hf_mq_xa_length, tvb, offset, 4, FALSE);
							proto_tree_add_item(mq_tree, hf_mq_xa_returnvalue, tvb, offset + 4, 4, bLittleEndian);

							/
							{
								proto_tree	*mq_tree_sub = NULL;
								guint32 iTMFlags;

								ti = proto_tree_add_item(mq_tree, hf_mq_xa_tmflags, tvb, offset + 8, 4, bLittleEndian);
								mq_tree_sub = proto_item_add_subtree(ti, ett_mq_xa_tmflags);
								iTMFlags = tvb_get_guint32_endian(tvb, offset + 8, bLittleEndian);

								proto_tree_add_boolean(mq_tree_sub, hf_mq_xa_tmflags_onephase, tvb, offset + 8, 4, iTMFlags);
								proto_tree_add_boolean(mq_tree_sub, hf_mq_xa_tmflags_fail, tvb, offset + 8, 4, iTMFlags);
								proto_tree_add_boolean(mq_tree_sub, hf_mq_xa_tmflags_resume, tvb, offset + 8, 4, iTMFlags);
								proto_tree_add_boolean(mq_tree_sub, hf_mq_xa_tmflags_success, tvb, offset + 8, 4, iTMFlags);
								proto_tree_add_boolean(mq_tree_sub, hf_mq_xa_tmflags_suspend, tvb, offset + 8, 4, iTMFlags);
								proto_tree_add_boolean(mq_tree_sub, hf_mq_xa_tmflags_startrscan, tvb, offset + 8, 4, iTMFlags);
								proto_tree_add_boolean(mq_tree_sub, hf_mq_xa_tmflags_endrscan, tvb, offset + 8, 4, iTMFlags);
								proto_tree_add_boolean(mq_tree_sub, hf_mq_xa_tmflags_join, tvb, offset + 8, 4, iTMFlags);
							}

							proto_tree_add_item(mq_tree, hf_mq_xa_rmid, tvb, offset + 12, 4, bLittleEndian);
						}
						offset += 16;
						if (opcode == MQ_TST_XA_START || opcode == MQ_TST_XA_END || opcode == MQ_TST_XA_PREPARE
							|| opcode == MQ_TST_XA_COMMIT || opcode == MQ_TST_XA_ROLLBACK || opcode == MQ_TST_XA_FORGET
							|| opcode == MQ_TST_XA_COMPLETE)
						{
							gint iSizeXid = 0;
							if ((iSizeXid = dissect_mq_xid(tvb, mqroot_tree, bLittleEndian, offset)) != 0)
								offset += iSizeXid;
						}
						else if ((opcode == MQ_TST_XA_OPEN || opcode == MQ_TST_XA_CLOSE)
							&& tvb_length_remaining(tvb, offset) >= 1)
						{
							guint8 iXAInfoLength = 0;
							iXAInfoLength = tvb_get_guint8(tvb, offset);
							if (tvb_length_remaining(tvb, offset) >= iXAInfoLength + 1)
							{
								if (tree)
								{
									ti = proto_tree_add_text(mqroot_tree, tvb, offset, iXAInfoLength + 1, MQ_TEXT_XINF);
									mq_tree = proto_item_add_subtree(ti, ett_mq_xa_info);

									proto_tree_add_item(mq_tree, hf_mq_xa_xainfo_length, tvb, offset, 1, FALSE);
									proto_tree_add_item(mq_tree, hf_mq_xa_xainfo_value, tvb, offset + 1, iXAInfoLength, FALSE);
								}
							}
							offset += 1 + iXAInfoLength;
						}
						else if ((opcode == MQ_TST_XA_RECOVER || opcode == MQ_TST_XA_RECOVER_REPLY)
							&& tvb_length_remaining(tvb, offset) >= 4)
						{
							gint iNbXid = 0;
							iNbXid = tvb_get_guint32_endian(tvb, offset, bLittleEndian);
							if (tree)
							{
								proto_tree_add_item(mq_tree, hf_mq_xa_count, tvb, offset, 4, bLittleEndian);
							}
							offset += 4;
							if (opcode == MQ_TST_XA_RECOVER_REPLY)
							{
								gint iXid = 0;
								for (iXid = 0; iXid < iNbXid; iXid++)
								{
									gint iSizeXid = 0;
									if ((iSizeXid = dissect_mq_xid(tvb, mqroot_tree, bLittleEndian, offset)) != 0)
										offset += iSizeXid;
									else
										break;
								}
							}
						}
					}
					else if ((structId == MQ_STRUCTID_ID || structId == MQ_STRUCTID_ID_EBCDIC) && tvb_length_remaining(tvb, offset) >= 5)
					{
						guint8 iVersionID = 0;
						gint iSizeID = 0;
						iVersionID = tvb_get_guint8(tvb, offset + 4);
						iSizeID = (iVersionID < 4 ? 44 : 104); /
						/

						if (iSizeID != 0 && tvb_length_remaining(tvb, offset) >= iSizeID)
						{
							if (check_col(pinfo->cinfo, COL_INFO))
							{
								guint8* sChannel;
								sChannel = tvb_get_ephemeral_string(tvb, offset + 24, 20);
								if (strip_trailing_blanks(sChannel, 20) != 0)
								{
									col_append_fstr(pinfo->cinfo, COL_INFO, ": CHL=%s", sChannel);
								}
							}

							if (tree)
							{
								ti = proto_tree_add_text(mqroot_tree, tvb, offset, iSizeID, MQ_TEXT_ID);
								mq_tree = proto_item_add_subtree(ti, ett_mq_id);

								proto_tree_add_item(mq_tree, hf_mq_id_structid, tvb, offset, 4, FALSE);
								proto_tree_add_item(mq_tree, hf_mq_id_level, tvb, offset + 4, 1, FALSE);

								/
								{
									proto_tree	*mq_tree_sub = NULL;
									guint8 iIDFlags;

									ti = proto_tree_add_item(mq_tree, hf_mq_id_flags, tvb, offset + 5, 1, FALSE);
									mq_tree_sub = proto_item_add_subtree(ti, ett_mq_id_icf);
									iIDFlags = tvb_get_guint8(tvb, offset + 5);

									proto_tree_add_boolean(mq_tree_sub, hf_mq_id_icf_runtime, tvb, offset + 5, 1, iIDFlags);
									proto_tree_add_boolean(mq_tree_sub, hf_mq_id_icf_svrsec, tvb, offset + 5, 1, iIDFlags);
									proto_tree_add_boolean(mq_tree_sub, hf_mq_id_icf_mqreq, tvb, offset + 5, 1, iIDFlags);
									proto_tree_add_boolean(mq_tree_sub, hf_mq_id_icf_splitmsg, tvb, offset + 5, 1, iIDFlags);
									proto_tree_add_boolean(mq_tree_sub, hf_mq_id_icf_convcap, tvb, offset + 5, 1, iIDFlags);
									proto_tree_add_boolean(mq_tree_sub, hf_mq_id_icf_msgseq, tvb, offset + 5, 1, iIDFlags);
								}

								proto_tree_add_item(mq_tree, hf_mq_id_unknown2, tvb, offset + 6, 1, FALSE);

								/
								{
									proto_tree	*mq_tree_sub = NULL;
									guint8 iErrorFlags;

									ti = proto_tree_add_item(mq_tree, hf_mq_id_ieflags, tvb, offset + 7, 1, FALSE);
									mq_tree_sub = proto_item_add_subtree(ti, ett_mq_id_ief);
									iErrorFlags = tvb_get_guint8(tvb, offset + 7);

									proto_tree_add_boolean(mq_tree_sub, hf_mq_id_ief_hbint, tvb, offset + 7, 1, iErrorFlags);
									proto_tree_add_boolean(mq_tree_sub, hf_mq_id_ief_seqwrap, tvb, offset + 7, 1, iErrorFlags);
									proto_tree_add_boolean(mq_tree_sub, hf_mq_id_ief_mxmsgpb, tvb, offset + 7, 1, iErrorFlags);
									proto_tree_add_boolean(mq_tree_sub, hf_mq_id_ief_mxmsgsz, tvb, offset + 7, 1, iErrorFlags);
									proto_tree_add_boolean(mq_tree_sub, hf_mq_id_ief_fap, tvb, offset + 7, 1, iErrorFlags);
									proto_tree_add_boolean(mq_tree_sub, hf_mq_id_ief_mxtrsz, tvb, offset + 7, 1, iErrorFlags);
									proto_tree_add_boolean(mq_tree_sub, hf_mq_id_ief_enc, tvb, offset + 7, 1, iErrorFlags);
									proto_tree_add_boolean(mq_tree_sub, hf_mq_id_ief_ccsid, tvb, offset + 7, 1, iErrorFlags);
								}

								proto_tree_add_item(mq_tree, hf_mq_id_unknown4, tvb, offset + 8, 2, FALSE);
								proto_tree_add_item(mq_tree, hf_mq_id_maxmsgperbatch, tvb, offset + 10, 2, bLittleEndian);
								proto_tree_add_item(mq_tree, hf_mq_id_maxtransmissionsize, tvb, offset + 12, 4, bLittleEndian);
								proto_tree_add_item(mq_tree, hf_mq_id_maxmsgsize, tvb, offset + 16, 4, bLittleEndian);
								proto_tree_add_item(mq_tree, hf_mq_id_sequencewrapvalue, tvb, offset + 20, 4, bLittleEndian);
								proto_tree_add_item(mq_tree, hf_mq_id_channel, tvb, offset + 24, 20, FALSE);
							}

							if (iVersionID >= 4)
							{
								if (check_col(pinfo->cinfo, COL_INFO))
								{
									guint8* sQueueManager;
									sQueueManager = tvb_get_ephemeral_string(tvb, offset + 48, 48);
									if (strip_trailing_blanks(sQueueManager,48) != 0)
									{
										col_append_fstr(pinfo->cinfo, COL_INFO, " QM=%s", sQueueManager);
									}
								}

								if (tree)
								{
									proto_tree_add_item(mq_tree, hf_mq_id_capflags, tvb, offset + 44, 1, FALSE);
									proto_tree_add_item(mq_tree, hf_mq_id_unknown5, tvb, offset + 45, 1, FALSE);
									proto_tree_add_item(mq_tree, hf_mq_id_ccsid, tvb, offset + 46, 2, bLittleEndian);
									proto_tree_add_item(mq_tree, hf_mq_id_queuemanager, tvb, offset + 48, 48, FALSE);
									proto_tree_add_item(mq_tree, hf_mq_id_heartbeatinterval, tvb, offset + 96, 4, bLittleEndian);
								}

							}
							offset += iSizeID;
						}
					}
					else if ((structId == MQ_STRUCTID_UID || structId == MQ_STRUCTID_UID_EBCDIC) && tvb_length_remaining(tvb, offset) > 0)
					{
						gint iSizeUID = 0;
						/
						/
						iSizeUID = iSegmentLength - iSizeTSH;
						if (iSizeUID != 28 && iSizeUID != 132) iSizeUID = 0;

						if (iSizeUID != 0 && tvb_length_remaining(tvb, offset) >= iSizeUID)
						{
							if (check_col(pinfo->cinfo, COL_INFO))
							{
								guint8* sUserId;
								sUserId = tvb_get_ephemeral_string(tvb, offset + 4, 12);
								if (strip_trailing_blanks(sUserId, 12) != 0)
								{
									col_append_fstr(pinfo->cinfo, COL_INFO, ": User=%s", sUserId);
								}
							}

							if (tree)
							{
								ti = proto_tree_add_text(mqroot_tree, tvb, offset, iSizeUID, MQ_TEXT_UID);
								mq_tree = proto_item_add_subtree(ti, ett_mq_uid);

								proto_tree_add_item(mq_tree, hf_mq_uid_structid, tvb, offset, 4, FALSE);
								proto_tree_add_item(mq_tree, hf_mq_uid_userid, tvb, offset + 4, 12, FALSE);
								proto_tree_add_item(mq_tree, hf_mq_uid_password, tvb, offset + 16, 12, FALSE);
							}

							if (iSizeUID == 132)
							{
								if (tree)
								{
									proto_tree_add_item(mq_tree, hf_mq_uid_longuserid, tvb, offset + 28, 64, FALSE);
									proto_tree_add_item(mq_tree, hf_mq_uid_securityid, tvb, offset + 92, 40, FALSE);
								}
							}
							offset += iSizeUID;
						}
					}
					if ((structId == MQ_STRUCTID_OD || structId == MQ_STRUCTID_OD_EBCDIC) && tvb_length_remaining(tvb, offset) >= 8)
					{
						/
						gint iSizeOD = 0;
						guint32 iVersionOD = 0;
						iVersionOD = tvb_get_guint32_endian(tvb, offset + 4, bLittleEndian);
						/
						switch (iVersionOD)
						{
							case 1: iSizeOD = 168; break;
							case 2: iSizeOD = 200; break;
							case 3: iSizeOD = 336; break;
						}

						if (iSizeOD != 0 && tvb_length_remaining(tvb, offset) >= iSizeOD)
						{
							gint iNbrRecords = 0;
							if (iVersionOD >= 2)
								iNbrRecords = tvb_get_guint32_endian(tvb, offset + 168, bLittleEndian);

							if (check_col(pinfo->cinfo, COL_INFO))
							{
								guint8* sQueue;
								sQueue = tvb_get_ephemeral_string(tvb, offset + 12, 48);
								if (strip_trailing_blanks(sQueue,48) != 0)
								{
									col_append_fstr(pinfo->cinfo, COL_INFO, " Obj=%s", sQueue);
								}
							}

							if (tree)
							{
								ti = proto_tree_add_text(mqroot_tree, tvb, offset, iSizeOD, MQ_TEXT_OD);
								mq_tree = proto_item_add_subtree(ti, ett_mq_od);

								proto_tree_add_item(mq_tree, hf_mq_od_structid, tvb, offset, 4, FALSE);
								proto_tree_add_item(mq_tree, hf_mq_od_version, tvb, offset + 4, 4, bLittleEndian);
								proto_tree_add_item(mq_tree, hf_mq_od_objecttype, tvb, offset + 8, 4, bLittleEndian);
								proto_tree_add_item(mq_tree, hf_mq_od_objectname, tvb, offset + 12, 48, FALSE);
								proto_tree_add_item(mq_tree, hf_mq_od_objectqmgrname, tvb, offset + 60, 48, FALSE);
								proto_tree_add_item(mq_tree, hf_mq_od_dynamicqname, tvb, offset + 108, 48, FALSE);
								proto_tree_add_item(mq_tree, hf_mq_od_alternateuserid, tvb, offset + 156, 12, FALSE);

								if (iVersionOD >= 2)
								{
									proto_tree_add_item(mq_tree, hf_mq_od_recspresent, tvb, offset + 168, 4, bLittleEndian);
									proto_tree_add_item(mq_tree, hf_mq_od_knowndestcount, tvb, offset + 172, 4, bLittleEndian);
									proto_tree_add_item(mq_tree, hf_mq_od_unknowndestcount, tvb, offset + 176, 4, bLittleEndian);
									proto_tree_add_item(mq_tree, hf_mq_od_invaliddestcount, tvb, offset + 180, 4, bLittleEndian);
									proto_tree_add_item(mq_tree, hf_mq_od_objectrecoffset, tvb, offset + 184, 4, bLittleEndian);
									proto_tree_add_item(mq_tree, hf_mq_od_responserecoffset, tvb, offset + 188, 4, bLittleEndian);
									proto_tree_add_item(mq_tree, hf_mq_od_objectrecptr, tvb, offset + 192, 4, bLittleEndian);
									proto_tree_add_item(mq_tree, hf_mq_od_responserecptr, tvb, offset + 196, 4, bLittleEndian);
								}

								if (iVersionOD >= 3)
								{
									proto_tree_add_item(mq_tree, hf_mq_od_alternatesecurityid, tvb, offset + 200, 40, FALSE);
									proto_tree_add_item(mq_tree, hf_mq_od_resolvedqname, tvb, offset + 240, 48, FALSE);
									proto_tree_add_item(mq_tree, hf_mq_od_resolvedqmgrname, tvb, offset + 288, 48, FALSE);
								}

							}
							offset += iSizeOD;

							if (iNbrRecords > 0)
							{
								gint iOffsetOR = 0;
								gint iOffsetRR = 0;
								gint iSizeORRR = 0;

								iDistributionListSize = iNbrRecords;
								iOffsetOR = tvb_get_guint32_endian(tvb, offset - iSizeOD + 184, bLittleEndian);
								iOffsetRR = tvb_get_guint32_endian(tvb, offset - iSizeOD + 188, bLittleEndian);
								if ((iSizeORRR = dissect_mq_or(tvb, mqroot_tree, offset, iNbrRecords, iOffsetOR)) != 0)
									offset += iSizeORRR;
								if ((iSizeORRR = dissect_mq_rr(tvb, mqroot_tree, bLittleEndian, offset, iNbrRecords, iOffsetRR)) != 0)
									offset += iSizeORRR;
							}
						}
						structId = (tvb_length_remaining(tvb, offset) >= 4) ? tvb_get_ntohl(tvb, offset) : MQ_STRUCTID_NULL;
					}
					if ((opcode == MQ_TST_MQOPEN || opcode == MQ_TST_MQCLOSE
						|| opcode == MQ_TST_MQOPEN_REPLY || opcode == MQ_TST_MQCLOSE_REPLY)
						&& tvb_length_remaining(tvb, offset) >= 4)
					{
						if (tree)
						{
							ti = proto_tree_add_text(mqroot_tree, tvb, offset, 4, MQ_TEXT_OPEN);
							mq_tree = proto_item_add_subtree(ti, ett_mq_open);
							proto_tree_add_item(mq_tree, hf_mq_open_options, tvb, offset, 4, bLittleEndian);
						}
						offset += 4;
						structId = (tvb_length_remaining(tvb, offset) >= 4) ? tvb_get_ntohl(tvb, offset) : MQ_STRUCTID_NULL;
					}
					if ((iSizeMD = dissect_mq_md(tvb, mqroot_tree, bLittleEndian, offset, &tMsgProps)) != 0)
					{
						gint iSizeGMO = 0;
						gint iSizePMO = 0;
						offset += iSizeMD;

						if ((iSizeGMO = dissect_mq_gmo(tvb, pinfo, mqroot_tree, bLittleEndian, offset)) != 0)
						{
							offset += iSizeGMO;
							bPayload = TRUE;
						}
						else if ((iSizePMO = dissect_mq_pmo(tvb, pinfo, mqroot_tree, bLittleEndian, offset, &iDistributionListSize)) != 0)
						{
							offset += iSizePMO;
							bPayload = TRUE;
						}
						if (tvb_length_remaining(tvb, offset) >= 4)
						{
							if (bPayload == TRUE)
							{
								iSizePayload = tvb_get_guint32_endian(tvb, offset, bLittleEndian);
								if (tree)
								{
									ti = proto_tree_add_text(mqroot_tree, tvb, offset, 4, MQ_TEXT_PUT);
									mq_tree = proto_item_add_subtree(ti, ett_mq_put);
									proto_tree_add_item(mq_tree, hf_mq_put_length, tvb, offset, 4, bLittleEndian);
								}
								offset += 4;
							}
						}
					}
					if (iDistributionListSize > 0)
					{
						if (check_col(pinfo->cinfo, COL_INFO))
							col_append_fstr(pinfo->cinfo, COL_INFO, " (Distribution List, Size=%d)", iDistributionListSize);
					}
					if (bPayload == TRUE)
					{
						if (iSizePayload != 0 && tvb_length_remaining(tvb, offset) > 0)
						{
							/
							guint32 iHeadersLength = 0;
							if (tvb_length_remaining(tvb, offset) >= 4)
							{
								gint iSizeMD = 0;
								structId = tvb_get_ntohl(tvb, offset);

								if ((structId == MQ_STRUCTID_XQH || structId == MQ_STRUCTID_XQH_EBCDIC) && tvb_length_remaining(tvb, offset) >= 104)
								{
									/
									gint iSizeXQH = 104;
									if (tree)
									{
										ti = proto_tree_add_text(mqroot_tree, tvb, offset, iSizeXQH, MQ_TEXT_XQH);
										mq_tree = proto_item_add_subtree(ti, ett_mq_xqh);

										proto_tree_add_item(mq_tree, hf_mq_xqh_structid, tvb, offset, 4, FALSE);
										proto_tree_add_item(mq_tree, hf_mq_xqh_version, tvb, offset + 4, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_xqh_remoteq, tvb, offset + 8, 48, FALSE);
										proto_tree_add_item(mq_tree, hf_mq_xqh_remoteqmgr, tvb, offset + 56, 48, FALSE);
									}
									offset += iSizeXQH;
									iHeadersLength += iSizeXQH;

									if ((iSizeMD = dissect_mq_md(tvb, mqroot_tree, bLittleEndian, offset, &tMsgProps)) != 0)
									{
										offset += iSizeMD;
										iHeadersLength += iSizeMD;
									}

									structId = (tvb_length_remaining(tvb, offset) >= 4) ? tvb_get_ntohl(tvb, offset) : MQ_STRUCTID_NULL;
								}
								if ((structId == MQ_STRUCTID_DH || structId == MQ_STRUCTID_DH_EBCDIC) && tvb_length_remaining(tvb, offset) >= 48)
								{
									/
									gint iSizeDH = 48;
									gint iNbrRecords = 0;
									guint32 iRecFlags = 0;

									iNbrRecords = tvb_get_guint32_endian(tvb, offset + 36, bLittleEndian);
									iRecFlags = tvb_get_guint32_endian(tvb, offset + 32, bLittleEndian);
									tMsgProps.iOffsetEncoding = offset + 12;
									tMsgProps.iOffsetCcsid    = offset + 16;
									tMsgProps.iOffsetFormat   = offset + 20;

									if (tree)
									{
										ti = proto_tree_add_text(mqroot_tree, tvb, offset, iSizeDH, MQ_TEXT_DH);
										mq_tree = proto_item_add_subtree(ti, ett_mq_dh);

										proto_tree_add_item(mq_tree, hf_mq_head_structid, tvb, offset, 4, FALSE);
										proto_tree_add_item(mq_tree, hf_mq_head_version, tvb, offset + 4, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_head_length, tvb, offset + 8, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_head_encoding, tvb, offset + 12, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_head_ccsid, tvb, offset + 16, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_head_format, tvb, offset + 20, 8, FALSE);
										proto_tree_add_item(mq_tree, hf_mq_head_flags, tvb, offset + 28, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_dh_putmsgrecfields, tvb, offset + 32, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_dh_recspresent, tvb, offset + 36, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_dh_objectrecoffset , tvb, offset + 40, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_dh_putmsgrecoffset, tvb, offset + 44, 4, bLittleEndian);
									}
									offset += iSizeDH;
									iHeadersLength += iSizeDH;

									if (iNbrRecords > 0)
									{
										gint iOffsetOR = 0;
										gint iOffsetPMR = 0;
										gint iSizeORPMR = 0;

										iOffsetOR = tvb_get_guint32_endian(tvb, offset - iSizeDH + 40, bLittleEndian);
										iOffsetPMR = tvb_get_guint32_endian(tvb, offset - iSizeDH + 44, bLittleEndian);
										if ((iSizeORPMR = dissect_mq_or(tvb, mqroot_tree, offset, iNbrRecords, iOffsetOR)) != 0)
										{
											offset += iSizeORPMR;
											iHeadersLength += iSizeORPMR;
										}
										if ((iSizeORPMR = dissect_mq_pmr(tvb, mqroot_tree, bLittleEndian, offset, iNbrRecords, iOffsetPMR, iRecFlags)) != 0)
										{
											offset += iSizeORPMR;
											iHeadersLength += iSizeORPMR;
										}
									}

									structId = (tvb_length_remaining(tvb, offset) >= 4) ? tvb_get_ntohl(tvb, offset) : MQ_STRUCTID_NULL;
								}
								if ((structId == MQ_STRUCTID_DLH || structId == MQ_STRUCTID_DLH_EBCDIC) && tvb_length_remaining(tvb, offset) >= 172)
								{
									/
									gint iSizeDLH = 172;
									tMsgProps.iOffsetEncoding = offset + 108;
									tMsgProps.iOffsetCcsid    = offset + 112;
									tMsgProps.iOffsetFormat   = offset + 116;
									if (tree)
									{
										ti = proto_tree_add_text(mqroot_tree, tvb, offset, iSizeDLH, MQ_TEXT_DLH);
										mq_tree = proto_item_add_subtree(ti, ett_mq_dlh);

										proto_tree_add_item(mq_tree, hf_mq_dlh_structid, tvb, offset, 4, FALSE);
										proto_tree_add_item(mq_tree, hf_mq_dlh_version, tvb, offset + 4, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_dlh_reason, tvb, offset + 8, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_dlh_destq, tvb, offset + 12, 48, FALSE);
										proto_tree_add_item(mq_tree, hf_mq_dlh_destqmgr, tvb, offset + 60, 48, FALSE);
										proto_tree_add_item(mq_tree, hf_mq_dlh_encoding, tvb, offset + 108, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_dlh_ccsid, tvb, offset + 112, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_dlh_format, tvb, offset + 116, 8, FALSE);
										proto_tree_add_item(mq_tree, hf_mq_dlh_putappltype, tvb, offset + 124, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_dlh_putapplname, tvb, offset + 128, 28, FALSE);
										proto_tree_add_item(mq_tree, hf_mq_dlh_putdate, tvb, offset + 156, 8, FALSE);
										proto_tree_add_item(mq_tree, hf_mq_dlh_puttime, tvb, offset + 164, 8, FALSE);
									}
									offset += iSizeDLH;
									iHeadersLength += iSizeDLH;
									structId = (tvb_length_remaining(tvb, offset) >= 4) ? tvb_get_ntohl(tvb, offset) : MQ_STRUCTID_NULL;
								}
								if ((structId == MQ_STRUCTID_MDE || structId == MQ_STRUCTID_MDE_EBCDIC) && tvb_length_remaining(tvb, offset) >= 72)
								{
									/
									gint iSizeMDE = 72;
									tMsgProps.iOffsetEncoding = offset + 12;
									tMsgProps.iOffsetCcsid    = offset + 16;
									tMsgProps.iOffsetFormat   = offset + 20;
									if (tree)
									{
										ti = proto_tree_add_text(mqroot_tree, tvb, offset, iSizeMDE, MQ_TEXT_MDE);
										mq_tree = proto_item_add_subtree(ti, ett_mq_mde);

										proto_tree_add_item(mq_tree, hf_mq_head_structid, tvb, offset, 4, FALSE);
										proto_tree_add_item(mq_tree, hf_mq_head_version, tvb, offset + 4, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_head_length, tvb, offset + 8, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_head_encoding, tvb, offset + 12, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_head_ccsid, tvb, offset + 16, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_head_format, tvb, offset + 20, 8, FALSE);
										proto_tree_add_item(mq_tree, hf_mq_head_flags, tvb, offset + 28, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_md_groupid, tvb, offset + 32, 24, FALSE);
										proto_tree_add_item(mq_tree, hf_mq_md_msgseqnumber, tvb, offset + 56, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_md_offset, tvb, offset + 60, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_md_msgflags, tvb, offset + 64, 4, bLittleEndian);
										proto_tree_add_item(mq_tree, hf_mq_md_originallength, tvb, offset + 68, 4, bLittleEndian);
									}
									offset += iSizeMDE;
									iHeadersLength += iSizeMDE;
									structId = (tvb_length_remaining(tvb, offset) >= 4) ? tvb_get_ntohl(tvb, offset) : MQ_STRUCTID_NULL;
								}
								if ((structId == MQ_STRUCTID_CIH || structId == MQ_STRUCTID_CIH_EBCDIC
									|| structId == MQ_STRUCTID_IIH || structId == MQ_STRUCTID_IIH_EBCDIC
									|| structId == MQ_STRUCTID_RFH || structId == MQ_STRUCTID_RFH_EBCDIC
									|| structId == MQ_STRUCTID_RMH || structId == MQ_STRUCTID_RMH_EBCDIC
									|| structId == MQ_STRUCTID_WIH || structId == MQ_STRUCTID_WIH_EBCDIC)
								 		&& tvb_length_remaining(tvb, offset) >= 12)
								{
									/
									/
									gint iSizeHeader = 0;
									iSizeHeader = tvb_get_guint32_endian(tvb, offset + 8, bLittleEndian);

								 	if (tvb_length_remaining(tvb, offset) >= iSizeHeader)
								 	{
										tMsgProps.iOffsetEncoding = offset + 12;
										tMsgProps.iOffsetCcsid    = offset + 16;
										tMsgProps.iOffsetFormat   = offset + 20;
								 		if (tree)
										{
											ti = proto_tree_add_text(mqroot_tree, tvb, offset, iSizeHeader, val_to_str(structId, mq_structid_vals, "Unknown (0x%08x)"));
											mq_tree = proto_item_add_subtree(ti, ett_mq_head);

											proto_tree_add_item(mq_tree, hf_mq_head_structid, tvb, offset, 4, FALSE);
											proto_tree_add_item(mq_tree, hf_mq_head_version, tvb, offset + 4, 4, bLittleEndian);
											proto_tree_add_item(mq_tree, hf_mq_head_length, tvb, offset + 8, 4, bLittleEndian);
											proto_tree_add_item(mq_tree, hf_mq_head_encoding, tvb, offset + 12, 4, bLittleEndian);
											proto_tree_add_item(mq_tree, hf_mq_head_ccsid, tvb, offset + 16, 4, bLittleEndian);
											proto_tree_add_item(mq_tree, hf_mq_head_format, tvb, offset + 20, 8, FALSE);
											proto_tree_add_item(mq_tree, hf_mq_head_flags, tvb, offset + 28, 4, bLittleEndian);
											proto_tree_add_item(mq_tree, hf_mq_head_struct, tvb, offset + 32, iSizeHeader - 32, bLittleEndian);

										}
										offset += iSizeHeader;
										iHeadersLength += iSizeHeader;
										structId = (tvb_length_remaining(tvb, offset) >= 4) ? tvb_get_ntohl(tvb, offset) : MQ_STRUCTID_NULL;
									}
								}
							}

							if (tMsgProps.iOffsetFormat != 0)
							{
									guint8* sFormat = NULL;
									sFormat = tvb_get_ephemeral_string(tvb, tMsgProps.iOffsetFormat, 8);
									if (strip_trailing_blanks(sFormat, 8) == 0)	sFormat = (guint8*)g_strdup("MQNONE");
									if (check_col(pinfo->cinfo, COL_INFO))
									{
										col_append_fstr(pinfo->cinfo, COL_INFO, " Fmt=%s", sFormat);
									}
									if (tree)
									{
										proto_tree_add_string_hidden(tree, hf_mq_md_hidden_lastformat, tvb, tMsgProps.iOffsetFormat, 8, (const char*)sFormat);
									}
							}
							if (check_col(pinfo->cinfo, COL_INFO))
							{
								col_append_fstr(pinfo->cinfo, COL_INFO, " (%d bytes)", iSizePayload - iHeadersLength);
							}

							{
								/
								tvbuff_t* next_tvb = NULL;
								struct mqinfo mqinfo;
								/
								mqinfo.encoding = tvb_get_guint32_endian(tvb, tMsgProps.iOffsetEncoding, bLittleEndian);
								mqinfo.ccsid    = tvb_get_guint32_endian(tvb, tMsgProps.iOffsetCcsid, bLittleEndian);
								tvb_memcpy(tvb, mqinfo.format, tMsgProps.iOffsetFormat, 8);
								pinfo->private_data = &mqinfo;
								next_tvb = tvb_new_subset(tvb, offset, -1, -1);
								if (!dissector_try_heuristic(mq_heur_subdissector_list, next_tvb, pinfo, tree))
									call_dissector(data_handle, next_tvb, pinfo, tree);
							}
						}
						offset = tvb_length(tvb);
					}
					/
					if (tvb_length_remaining(tvb, offset) >= 4)
					{
						structId = tvb_get_ntohl(tvb, offset);
						if (tree)
						{
							proto_tree_add_text(mqroot_tree, tvb, offset, -1, val_to_str(structId, mq_structid_vals, "Unknown (0x%08x)"));
						}
					}
				}
				else
				{
					/
					if (check_col(pinfo->cinfo, COL_INFO)) col_append_str(pinfo->cinfo, COL_INFO, " [Unreassembled MQ]");
					call_dissector(data_handle, tvb_new_subset(tvb, offset, -1, -1), pinfo, tree);
				}
			}
		}
		else
		{
			/
			if (check_col(pinfo->cinfo, COL_INFO)) col_append_str(pinfo->cinfo, COL_INFO, " [Undesegmented]");
			if (tree)
			{
				proto_tree_add_item(tree, proto_mq, tvb, offset, -1, FALSE);
			}
			call_dissector(data_handle, tvb_new_subset(tvb, offset, -1, -1), pinfo, tree);
		}
	}
}
