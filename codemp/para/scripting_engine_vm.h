#ifndef SCRIPTING_ENGINE_VM_H
#define SCRIPTING_ENGINE_VM_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct para_seImport_s {
	void				(*Ping)						( void );
	void				(*Event_ChatMsg)			( char const * name, char const * msg );
} para_seImport_t;

#ifdef __cplusplus
}
#endif
#endif //SCRIPTING_ENGINE_VM_H
