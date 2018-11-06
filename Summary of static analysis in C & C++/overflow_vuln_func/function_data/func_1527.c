static char *
wv_csp13_opaque_literal_tag(tvbuff_t *tvb, guint32 offset,
			    const char *token, guint8 codepage _U_, guint32 *length)
{
	guint32 data_len = tvb_get_guintvar(tvb, offset, length);
	char *str = NULL;

	if ( token && ( (strcmp(token, "Code") == 0)
			|| (strcmp(token, "ContentSize") == 0)
			|| (strcmp(token, "MessageCount") == 0)
			|| (strcmp(token, "Validity") == 0)
			|| (strcmp(token, "KeepAliveTime") == 0)
			|| (strcmp(token, "TimeToLive") == 0)
			|| (strcmp(token, "AcceptedContentLength") == 0)
			|| (strcmp(token, "MultiTrans") == 0)
			|| (strcmp(token, "ParserSize") == 0)
			|| (strcmp(token, "ServerPollMin") == 0)
			|| (strcmp(token, "TCPPort") == 0)
			|| (strcmp(token, "UDPPort") == 0)
			|| (strcmp(token, "HistoryPeriod") == 0)
			|| (strcmp(token, "MaxWatcherList") == 0)
			/
			|| (strcmp(token, "SearchFindings") == 0)
			|| (strcmp(token, "SearchID") == 0)
			|| (strcmp(token, "SearchIndex") == 0)
			|| (strcmp(token, "SearchLimit") == 0)
			|| (strcmp(token, "AcceptedPullLength") == 0)
			|| (strcmp(token, "AcceptedPushLength") == 0)
			|| (strcmp(token, "AcceptedRichContentLength") == 0)
			|| (strcmp(token, "AcceptedTextContentLength") == 0)
			|| (strcmp(token, "SessionPriority") == 0)
			|| (strcmp(token, "UserSessionLimit") == 0)
			|| (strcmp(token, "MultiTransPerMessage") == 0)
			|| (strcmp(token, "ContentPolicyLimit") == 0)
			|| (strcmp(token, "AnswerOptionID") == 0)
			|| (strcmp(token, "SegmentCount") == 0)
			|| (strcmp(token, "SegmentReference") == 0)
			|| (strcmp(token, "TryAgainTimeout") == 0)
			|| (strcmp(token, "GroupContentLimit") == 0)
			|| (strcmp(token, "MessageTotalCount") == 0)
			|| (strcmp(token, "PairID") == 0) ) )
		{
			str = wv_integer_from_opaque(tvb, offset + *length, data_len);
		}
	else
		if ( token && ( (strcmp(token, "DateTime") == 0)
				|| (strcmp(token, "DeliveryTime") == 0) ) )
			{
				str = wv_datetime_from_opaque(tvb, offset + *length, data_len);
			}

	if (str == NULL) { /
		str = wmem_strdup_printf(wmem_packet_scope(), "(%d bytes of unparsed opaque data)", data_len);
	}
	*length += data_len;
	return str;
}
