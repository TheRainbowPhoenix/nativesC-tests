# Checksums are SHA256
# Side note for future self or maintainers:
#    'missing_byte_X_offset' are the missing bit found around loc_10003A3B and loc_10003A72 
#    They are basically that byte value, offset from the DLL content.
#    0x2E88 means the 11912 th bit from the OSUpdateDLL, that is the same as the one found on the updater.
#    Dunno why it's done that way and not hardcoding missing bits, maybe it's a DRM ?
#    Won't change for now, as hash guarantee we're all using the same file...
VERSION_SPECIFIC_INFO = {
	'v02.01.2000.0000': {
		'OSupdateDLL_checksum': '22DC7E129F8840E8BAB9490EE96D05C825C98E6060DD27FC66B523CE67180EDA',
		'missing_byte_3069_offset': 0x2E88,
		'missing_byte_3070_offset': 0x2E51
	},
	'v02.01.4000.0000': {
		'OSupdateDLL_checksum': '06C8E87CA6E4A8463212141A657D01BD373D78ED84EBCF555891C27B391D60E3',
		'missing_byte_3069_offset': 0x2E88,
		'missing_byte_3070_offset': 0x2E51
	},
	'v02.01.5000.0000': {
		'OSupdateDLL_checksum': '1B9296674C65645AF403DD5D21A2EB8FD231B2DC6E716BB23E3F39F77A916BF7',
		'missing_byte_3069_offset': 0x2E88,
		'missing_byte_3070_offset': 0x2E51
	},
	'v02.01.6000.0000': {
		'OSupdateDLL_checksum': 'E26F2366AC485931F51592DCE4119570F2207ECCC3762402624FB5D0DBE54409',
		'missing_byte_3069_offset': 0x2E88,
		'missing_byte_3070_offset': 0x2E51
	},
	'v02.01.7001.0000': {
		'OSupdateDLL_checksum': '52CC500A65CD495AB9C0FC642C06510670675A57EAA2EAB15E8A744BBE46C67F',
		'missing_byte_3069_offset': 0x2E88,
		'missing_byte_3070_offset': 0x2E51
	},
	'v02.01.7002.0001': {
		'OSupdateDLL_checksum': '93BA8B21BB39F1151D6C12A8D0353AC4DD211EE6E60460D1C734A7CFA54F4B37',
		'missing_byte_3069_offset': 0x2E57, # ABS changed
		'missing_byte_3070_offset': 0x2E55
	},
	'v02.01.7003.0000': {
		'OSupdateDLL_checksum': '642C589EE75CF5277061426B8B7A7D78C086B2AC5A4136195120F03F3A572D36',
		'missing_byte_3069_offset': 0x2E6D,
		'missing_byte_3070_offset': 0x3335
	},
	# Critor gonna love it
	# '3.10.2000.0': {
	# 	'OSupdateDLL_checksum': '4AF17E59EBCD5ED50C8363C9258D07ABCB6420CCB96232A2A34AED132ED666FE',
	# 	'missing_byte_3069_offset': 0x377D,
	# 	'missing_byte_3070_offset': 0x20E7,
	# 	'missing_byte_offset': 0x3000
	# },
	'3.10.4000.0': {
		'OSupdateDLL_checksum': '9B1E46ADD718E7515A6C2312DB572CBAE063C63BF2AAA68767231750BE0300F8',
		'missing_byte_3069_offset': 0x2D04,
		'missing_byte_3070_offset': 0x2B99
	},
	'3.10.5000.0': {
		'OSupdateDLL_checksum': '2FC7AD5759D626DB399E61F6A1543D9DF00D89626A65DDFBB403510D8864D0C8',
		'missing_byte_3069_offset': 0x2D04,
		'missing_byte_3070_offset': 0x2E21
	},
	'3.10.6000.0': {
		'OSupdateDLL_checksum': '61D2D881817C562125A98AF577549918212AB2C1C24209C57EADF3463BAF37D3',
		'missing_byte_3069_offset': 0x2D04,
		'missing_byte_3070_offset': 0x2E21
	},
	'3.10.7000.0': {
		'OSupdateDLL_checksum': '3D45ED7A0665093CD83D07909675FC0E84A7BF124701EAE3DED3BD5B82CC8B14',
		'missing_byte_3069_offset': 0x2D04, 
		'missing_byte_3070_offset': 0x2B99
	}
}

COMPRESSED_IMAGE_HEADER = [0x1F, 0x8B, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00]
MISSING_BYTE_OFFSET = 0x2FF6

GZIP_COMPRESSION_LEVEL = 6

def find_version_from_OSupdateDLL_checksum(checksum):
	"""Determines the firmware version with the checksum of the OSupdateDLL.dll file.

	Args:
		checksum: The SHA256 checksum of the OSupdateDLL.dll file.

	Returns:
		The version, or None if the checksum did not match any known version.
	"""

	for version, data in VERSION_SPECIFIC_INFO.items():
		if data['OSupdateDLL_checksum'].lower() == checksum.lower():
			return version
