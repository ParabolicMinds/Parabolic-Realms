#ifndef PSE_LOADER_H
#define PSE_LOADER_H

typedef struct pseImport_s {
	void *			handle;
	char const *	(*Identify)			( void );
	void			(*Ping)				( void );
	void			(*Event_ChatMsg)	( char const * name, char const * msg );
} pseImport_t;

pseImport_t * PSE_LoadLibrary(char * const path);
void PSE_UnloadLibrary(pseImport_t ** import);

#endif //PSE_LOADER_H
