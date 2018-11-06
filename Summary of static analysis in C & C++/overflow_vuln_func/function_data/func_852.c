static void
fTags (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset, gint service_choice)
{
	guint8 offs, tag_no, class_tag;
	guint32 lvt;

	if ((*offset) >= tvb_reported_length(tvb))
		return;

	offs = fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

#if 0
#if 0
	if (class_tag)	{	/
#endif
		switch (service_choice) {
		case 0:	/
			fAckAlarm (tvb, pinfo, tree, offset); /
			break;
		case 1: /
			fCOVNotification (tvb, pinfo, tree, offset); /
		break;
		case 2: /
			fEventNotification (tvb, pinfo, tree, offset); /
		break;
		case 3: /
			fGetAlarmSummary (tvb, pinfo, tree, offset); /
		break;
		case 4: /
			fgetEnrollmentSummaryAck (tvb, pinfo, tree, offset);
			break;
		case 5: /
			fSubscribeCOV (tvb, pinfo, tree, offset);
		break;
		case 6: /
			fAtomicReadFile (tvb, tree, offset);
			break;
		case 7: /
			fAtomicWriteFileAck (tvb, tree, offset);
			break;
		case 8: /
			fAddListElement (tvb, pinfo, tree, offset);
			break;
		case 9: /
			fRemoveListElement (tvb, pinfo, tree, offset);
			break;
		case 10: /
			fCreateObjectAck (tvb, tree, offset);
		break;
		case 11: /
			fDeleteObject (tvb, tree, offset);
		break;
		case 12:
			fReadWriteProperty (tvb, pinfo, tree, offset); /
			break;
		case 13:
			fReadPropertyConditionalAck (tvb, pinfo, tree, offset); /
			break;
		case 14:
			fReadPropertyMultipleAck (tvb, pinfo, tree, offset); /
			break;
		case 15:
			fReadWriteProperty (tvb, pinfo, tree, offset); /
			break;
		case 16:
			fReadWriteMultipleProperty (tvb, pinfo, tree, offset); /
			break;
		case 17:
			fDeviceCommunicationControl (tvb, pinfo, tree, offset); /
			break;
		case 18:
			fPrivateTransfer (tvb, pinfo, tree, offset); /
			break;
		case 19:
			fTextMessage (tvb, pinfo, tree, offset); /
			break;
		case 20:
			fReinitializeDevice (tvb, pinfo, tree, offset); /
			break;
		case 21:
			fVtOpen (tvb, pinfo, tree, offset); /
			break;
		case 22:
			fVtClose (tvb, pinfo, tree, offset); /
			break;
		case 23:
			fVtData (tvb, pinfo, tree, offset); /
			break;
		case 24:
			fAuthenticate (tvb, pinfo, tree, offset); /
			break;
		case 25:
			fRequestKey (tvb, pinfo, tree, offset); /
			break;
		case 26:
			fReadRangeAck (tvb, pinfo, tree, offset); /
			break;
		case 27:
			fLifeSafetyOperation (tvb, pinfo, tree, offset); /
			break;
		case 28:
			fSubscribeCOVProperty (tvb, pinfo, tree, offset); /
			break;
		case 29:
			fGetEventInformation (tvb, pinfo, tree, offset); /
			break;
		default:
			return;
		break;
		}
#if 0
			break;
		default:
			return;
		break;
		}
#if 0
	} else {	/
		fApplicationTags (tvb, pinfo, tree, offset, NULL, NULL);
	}
#endif
}
