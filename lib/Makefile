TOPLEVEL = ..
include vars.mk

INCLUDES_DIR = ./include
MXF_DIR = ./mxf
PRODUCTS_DIR = ./products
UTILS_DIR = ./utils

OBJS = $(MXF_DIR)/mxf_version.o \
	$(MXF_DIR)/mxf_labels_and_keys.o \
	$(MXF_DIR)/mxf_list.o \
	$(MXF_DIR)/mxf_utils.o \
	$(MXF_DIR)/mxf_logging.o \
	$(MXF_DIR)/mxf_file.o \
	$(MXF_DIR)/mxf_partition.o \
	$(MXF_DIR)/mxf_primer.o \
	$(MXF_DIR)/mxf_essence_container.o \
	$(MXF_DIR)/mxf_index_table.o \
	$(MXF_DIR)/mxf_data_model.o \
	$(MXF_DIR)/mxf_header_metadata.o \
	$(PRODUCTS_DIR)/mxf_avid.o \
	$(PRODUCTS_DIR)/mxf_avid_metadictionary.o \
	$(PRODUCTS_DIR)/mxf_avid_dictionary.o \
	$(PRODUCTS_DIR)/mxf_p2.o \
	$(UTILS_DIR)/mxf_uu_metadata.o \
	$(UTILS_DIR)/mxf_page_file.o

INCLUDE_FILES = $(INCLUDES_DIR)/mxf/mxf_data_model.h \
	$(INCLUDES_DIR)/mxf/mxf_header_metadata.h \
	$(INCLUDES_DIR)/mxf/mxf_baseline_data_model.h \
	$(INCLUDES_DIR)/mxf/mxf_extensions_data_model.h \
	$(INCLUDES_DIR)/mxf/mxf_partition.h \
	$(INCLUDES_DIR)/mxf/mxf_primer.h \
	$(INCLUDES_DIR)/mxf/mxf_essence_container.h \
	$(INCLUDES_DIR)/mxf/mxf_index_table.h \
	$(INCLUDES_DIR)/mxf/mxf_labels_and_keys.h \
	$(INCLUDES_DIR)/mxf/mxf_list.h \
	$(INCLUDES_DIR)/mxf/mxf_logging.h \
	$(INCLUDES_DIR)/mxf/mxf_utils.h \
	$(INCLUDES_DIR)/mxf/mxf_page_file.h \
	$(INCLUDES_DIR)/mxf/mxf_file.h \
	$(INCLUDES_DIR)/mxf/mxf_version.h \
	$(INCLUDES_DIR)/mxf/mxf_types.h \
	$(INCLUDES_DIR)/mxf/mxf_macros.h \
	$(INCLUDES_DIR)/mxf/mxf.h \
	$(INCLUDES_DIR)/mxf/mxf_avid.h \
	$(INCLUDES_DIR)/mxf/mxf_avid_metadictionary.h \
	$(INCLUDES_DIR)/mxf/mxf_avid_dictionary.h \
	$(INCLUDES_DIR)/mxf/mxf_avid_extensions_data_model.h \
	$(INCLUDES_DIR)/mxf/mxf_avid_labels_and_keys.h \
	$(INCLUDES_DIR)/mxf/mxf_p2.h \
	$(INCLUDES_DIR)/mxf/mxf_p2_extensions_data_model.h \
	$(INCLUDES_DIR)/mxf/mxf_p2_labels_and_keys.h \
	$(INCLUDES_DIR)/mxf/mxf_uu_metadata.h




.PHONY: all
all: libMXF.a


libMXF.a: $(OBJS)
	$(AR) libMXF.a $(OBJS)


$(MXF_DIR)/mxf_essence_container.o: $(MXF_DIR)/mxf_essence_container.c $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) $(MXF_DIR)/mxf_essence_container.c -o $(MXF_DIR)/mxf_essence_container.o

$(MXF_DIR)/mxf_index_table.o: $(MXF_DIR)/mxf_index_table.c $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) $(MXF_DIR)/mxf_index_table.c -o $(MXF_DIR)/mxf_index_table.o

$(MXF_DIR)/mxf_partition.o: $(MXF_DIR)/mxf_partition.c $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) $(MXF_DIR)/mxf_partition.c -o $(MXF_DIR)/mxf_partition.o

$(MXF_DIR)/mxf_primer.o: $(MXF_DIR)/mxf_primer.c $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) $(MXF_DIR)/mxf_primer.c -o $(MXF_DIR)/mxf_primer.o

$(MXF_DIR)/mxf_header_metadata.o: $(MXF_DIR)/mxf_header_metadata.c $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) $(MXF_DIR)/mxf_header_metadata.c -o $(MXF_DIR)/mxf_header_metadata.o

$(MXF_DIR)/mxf_data_model.o: $(MXF_DIR)/mxf_data_model.c $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) $(MXF_DIR)/mxf_data_model.c -o $(MXF_DIR)/mxf_data_model.o

$(MXF_DIR)/mxf_labels_and_keys.o: $(MXF_DIR)/mxf_labels_and_keys.c $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) $(MXF_DIR)/mxf_labels_and_keys.c -o $(MXF_DIR)/mxf_labels_and_keys.o

$(MXF_DIR)/mxf_utils.o: $(MXF_DIR)/mxf_utils.c $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) $(MXF_DIR)/mxf_utils.c -o $(MXF_DIR)/mxf_utils.o

$(MXF_DIR)/mxf_list.o: $(MXF_DIR)/mxf_list.c $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) $(MXF_DIR)/mxf_list.c -o $(MXF_DIR)/mxf_list.o

$(MXF_DIR)/mxf_file.o: $(MXF_DIR)/mxf_file.c $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) $(MXF_DIR)/mxf_file.c -o $(MXF_DIR)/mxf_file.o

$(MXF_DIR)/mxf_logging.o: $(MXF_DIR)/mxf_logging.c $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) $(MXF_DIR)/mxf_logging.c -o $(MXF_DIR)/mxf_logging.o

$(MXF_DIR)/mxf_version.o: $(MXF_DIR)/mxf_version.c $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) $(MXF_DIR)/mxf_version.c -o $(MXF_DIR)/mxf_version.o


$(PRODUCTS_DIR)/mxf_avid_dictionary.o: $(PRODUCTS_DIR)/mxf_avid_dictionary.c $(INCLUDE_FILES) $(PRODUCTS_DIR)/mxf_avid_dictionary_data.h
	$(CC) -c $(CFLAGS) $(PRODUCTS_DIR)/mxf_avid_dictionary.c -o $(PRODUCTS_DIR)/mxf_avid_dictionary.o

$(PRODUCTS_DIR)/mxf_avid_metadictionary.o: $(PRODUCTS_DIR)/mxf_avid_metadictionary.c $(INCLUDE_FILES) $(PRODUCTS_DIR)/mxf_avid_metadictionary_data.h
	$(CC) -c $(CFLAGS) $(PRODUCTS_DIR)/mxf_avid_metadictionary.c -o $(PRODUCTS_DIR)/mxf_avid_metadictionary.o

$(PRODUCTS_DIR)/mxf_avid.o: $(PRODUCTS_DIR)/mxf_avid.c $(INCLUDE_FILES) $(AVID_INTERNAL_INCLUDE_FILES)
	$(CC) -c $(CFLAGS) $(PRODUCTS_DIR)/mxf_avid.c -o $(PRODUCTS_DIR)/mxf_avid.o 

$(PRODUCTS_DIR)/mxf_p2.o: $(PRODUCTS_DIR)/mxf_p2.c $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) $(PRODUCTS_DIR)/mxf_p2.c -o $(PRODUCTS_DIR)/mxf_p2.o 


$(UTILS_DIR)/mxf_uu_metadata.o: $(UTILS_DIR)/mxf_uu_metadata.c $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) $(UTILS_DIR)/mxf_uu_metadata.c -o $(UTILS_DIR)/mxf_uu_metadata.o 

$(UTILS_DIR)/mxf_page_file.o: $(UTILS_DIR)/mxf_page_file.c $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) $(UTILS_DIR)/mxf_page_file.c -o $(UTILS_DIR)/mxf_page_file.o 




.PHONY: install
install: libMXF.a
	mkdir -p $(MXF_INSTALL_PREFIX)/lib
	cp libMXF.a $(MXF_INSTALL_PREFIX)/lib
	mkdir -p $(MXF_INSTALL_PREFIX)/include/mxf
	cp $(INCLUDE_FILES) $(MXF_INSTALL_PREFIX)/include/mxf/

.PHONY: uninstall
uninstall:
	rm -f $(MXF_INSTALL_PREFIX)/lib/libMXF.a
	rm -fR $(MXF_INSTALL_PREFIX)/include/mxf


clean:
	@rm -f *~ *.a
	@cd $(INCLUDES_DIR)/mxf && rm -f *~
	@cd $(MXF_DIR) && rm -f *~ *.o 
	@cd $(PRODUCTS_DIR) && rm -f *~ *.o 
	@cd $(UTILS_DIR) && rm -f *~ *.o 

