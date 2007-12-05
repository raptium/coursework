bin_PROGRAMS = stock_srv stock_client
noinst_LIBRARIES = libsymtab.a liblist.a
libsymtab_a_SOURCES = symtab.c symtab.h
liblist_a_SOURCES = list.c list.h
stock_srv_SOURCES = server.c
stock_client_SOURCES = client.c
stock_client_LDADD = libsymtab.a liblist.a
