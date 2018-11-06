typedef struct  {
  const gchar *sa; 
  gint typ;
  gchar *src;
  gint src_len;
  gchar *dst;
  gint dst_len;
  gchar *spi;
  gint encryption_algo; 
  gint authentication_algo; 
  const gchar *encryption_key; 
  const gchar *authentication_key;    
  gboolean is_valid;
} g_esp_sa;
