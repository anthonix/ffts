void define_transform_size(ffts_plan_t *p, size_t N) {
	if(N < 8192) p->transform_size = 8192;
	else p->transform_size = N;
}

insns_t generate_size8_base_case(insns_t **fp, int sign ) { 
	memcpy(*fp, neon_x8, neon_x8_t - neon_x8);
	/* 
	 * Changes adds to subtracts and  vice versa to allow the computation 
	 * of both the IFFT and FFT
	 */
	if(sign < 0) {
		*fp[31] ^= 0x00200000; *fp[32] ^= 0x00200000; *fp[33] ^= 0x00200000; *fp[34] ^= 0x00200000;
		*fp[65] ^= 0x00200000; *fp[66] ^= 0x00200000; *fp[70] ^= 0x00200000; *fp[74] ^= 0x00200000;
		*fp[97] ^= 0x00200000; *fp[98] ^= 0x00200000; *fp[102] ^= 0x00200000; *fp[104] ^= 0x00200000;
	}
	*fp += (neon_x8_t - neon_x8) / 4;
}


insns_t generate_size4_base_case(insns_t **fp, int sign) {
		memcpy(*fp, neon_x4, neon_x8 - neon_x4);
		if(sign < 0) {
			*fp[26] ^= 0x00200000; *fp[28] ^= 0x00200000; *fp[31] ^= 0x00200000; *fp[32] ^= 0x00200000;
		}
		*fp += (neon_x8 - neon_x4) / 4;
}
