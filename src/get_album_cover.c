#include <id3tag.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void createAlbumImage(char *fileName, bool *isSuccessful) {
  *isSuccessful = false;

  struct id3_file *file = id3_file_open(fileName, ID3_FILE_MODE_READONLY);
  if (!file) {
    fprintf(stderr, "Error: Could not open %s\n", fileName);
    return;
  }

  struct id3_tag *tag = id3_file_tag(file);
  if (!tag) {
    fprintf(stderr, "Error: No ID3 tag found.\n");
    id3_file_close(file);
    return;
  }

  struct id3_frame *frame = id3_tag_findframe(tag, "APIC", 0);
  if (!frame) {
    fprintf(stderr, "Error: No album art frame found.\n");
    id3_file_close(file);
    return;
  }

  // APIC fieldsfor mp3 metadata: [0]=Text Encoding, [1]=MIME Type, [2]=Picture
  // Type, [3]=Description, [4]=Binary Data
  union id3_field *data_field = id3_frame_field(frame, 4);
  if (!data_field) {
    id3_file_close(file);
    return;
  }

  id3_length_t length = 0;
  id3_byte_t const *data = id3_field_getbinarydata(data_field, &length);
  if (!data || length == 0) {
    id3_file_close(file);
    return;
  }

  FILE *img_file = fopen("cover_raw", "wb");
  if (!img_file) {
    id3_file_close(file);
    return;
  }
  fwrite(data, 1, length, img_file);
  fclose(img_file);

  id3_file_close(file);

  system("sips -s format png cover_raw --out cover.png >/dev/null 2>&1");
  *isSuccessful = true;
}
