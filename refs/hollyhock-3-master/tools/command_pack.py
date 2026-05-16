import constants
import gzip
import hashlib
import pefile
from io import BytesIO
import os

def set_resource_rom(pe_file: str, recompressed_rom: bytes) -> None:
	pe = pefile.PE(pe_file)
	if hasattr(pe, 'DIRECTORY_ENTRY_RESOURCE'):
		# Handle ROM resource (3070)
		for resource_type in pe.DIRECTORY_ENTRY_RESOURCE.entries:
			if pefile.RESOURCE_TYPE.get(resource_type.struct.Id, '-') == 'RT_RCDATA':
				if hasattr(resource_type, 'directory'):
					for resource_id in resource_type.directory.entries:
						if hasattr(resource_id, 'directory'):
							for resource_item in resource_id.directory.entries:
								if resource_id.id == 3070:
									ROM_offset = resource_item.data.struct.OffsetToData
									ROM_size = resource_item.data.struct.Size
									print(f"ROM @ {ROM_offset}:{ROM_size}")
									
									pe.set_bytes_at_rva(ROM_offset, recompressed_rom)
									resource_item.data.struct.Size = len(recompressed_rom)

		# Handle Bitmap resource (3046)
		bitmap_path = os.path.join(os.path.dirname(pe_file), "Bitmap3046.bmp")
		if os.path.exists(bitmap_path):
			print(f"Found Bitmap3046.bmp, applying patch...")
			with open(bitmap_path, 'rb') as f:
				bitmap_data = f.read()

			for resource_type in pe.DIRECTORY_ENTRY_RESOURCE.entries:
				if pefile.RESOURCE_TYPE.get(resource_type.struct.Id, '-') == 'RT_BITMAP':
					if hasattr(resource_type, 'directory'):
						for resource_id in resource_type.directory.entries:
							if hasattr(resource_id, 'directory'):
								for resource_item in resource_id.directory.entries:
									if resource_id.id == 3046:

										bitmap_offset = resource_item.data.struct.OffsetToData
										data_rva = bitmap_offset
										offset = pe.get_offset_from_rva(bitmap_offset)
										bitmap_size = resource_item.data.struct.Size
										print(f'Directory entry at RVA{hex(data_rva)} of size {hex(bitmap_size)}')
										
										data = pe.get_memory_mapped_image()[data_rva:data_rva+bitmap_size]

										# with open("data_3046.bin", "wb") as f:
										# 	f.write(data)

										bitmap_data = bitmap_data[14:] # truncate the 14 first bytes
										new_size = len(bitmap_data)

										if new_size != bitmap_size:
											print(f"Size mismatch: New {new_size} vs Original {bitmap_size}. Aborting.")
											break

										print(f"Bitmap @ {hex(bitmap_offset)}:{hex(bitmap_size)}")										
										pe.set_bytes_at_offset(offset, bitmap_data)
										resource_item.data.struct.Size = new_size
										print(f"Replaced bitmap resource {resource_id.id} at offset {hex(offset)}.")

		pe.OPTIONAL_HEADER.CheckSum = pe.generate_checksum()
		output_path = os.path.splitext(pe_file)[0] + "_patch.dll"
		pe.write(output_path)
		return output_path

def go(args):
	with open(args.dll_path, 'rb') as f:
		dll_data = f.read()

		m = hashlib.sha256()
		m.update(dll_data)
		dll_checksum = m.hexdigest()

	version = constants.find_version_from_OSupdateDLL_checksum(dll_checksum)
	if version == None:
		print('Could not determine the firmware version associated with the specified OSupdateDLL.dll file.\nAborting.')
		return

	print(f'Found OSupdateDLL.dll firmware version: {version}\n')

	print('Reading image...', flush=True)
	with open(args.fw3070_path, 'rb') as f:
		fw3070_data = f.read()
	print('Done!\n')

	print('Packing image...', flush=True)
	fw3070c_data = gzip.compress(fw3070_data, constants.GZIP_COMPRESSION_LEVEL)

	# remove the header
	fw3070c_data = fw3070c_data[len(constants.COMPRESSED_IMAGE_HEADER):]

	# remove our missing byte
	missing_byte = fw3070c_data[constants.MISSING_BYTE_OFFSET]
	fw3070c_data = fw3070c_data[:constants.MISSING_BYTE_OFFSET] + fw3070c_data[(constants.MISSING_BYTE_OFFSET + 1):]

	print(f'    Missing byte: {missing_byte}')

	if missing_byte != dll_data[constants.VERSION_SPECIFIC_INFO[version]['missing_byte_3070_offset']]:
		print('This doesn\'t match the missing byte in your DLL. Since we aren\'t modifying the DLL, we can\'t continue.\nAborting.')
		return

	print('Packed!\n')

	print('Writing packed image...', flush=True)
	with open(args.packed_fw3070_path, 'wb') as f:
		f.write(fw3070c_data)

	print('Creating patched DLL...', flush=True)
	output_dll = set_resource_rom(args.dll_path, fw3070c_data)
	print(f'Patched DLL saved as: {output_dll}')
	print('Done!')

