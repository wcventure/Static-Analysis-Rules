unsigned int
rpc_roundup(unsigned int a)
{
	unsigned int mod = a % 4;
	return a + ((mod)? 4-mod : 0);
}
