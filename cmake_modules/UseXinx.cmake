set(CMAKE_C_FLAGS "-frtti -fexceptions -Wall")
set(CMAKE_CXX_FLAGS "-frtti -fexceptions -Wall") # -Wold-style-cast") # -Woverloaded-virtual")

if (WIN32)
	add_definitions(-DQT_LARGEFILE_SUPPORT)
	set(CMAKE_SHARED_LINKER_FLAGS " -mthreads")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mthreads")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mthreads") # -Wold-style-cast") # -Woverloaded-virtual")
endif (WIN32)

add_definitions(-DUNICODE)
add_definitions(-DQT_THREAD_SUPPORT)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
	add_definitions(-DQT_NO_DEBUG)
	add_definitions(-D_XINX_RELEASE_MODE_)
else()
	add_definitions(-DQT_SHAREDPOINTER_TRACK_POINTERS)
endif()


include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(MINGW)
	set(CMAKE_RC_COMPILER_INIT windres)
	enable_language(RC)
	set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> <DEFINES> -O coff -o <OBJECT> <SOURCE>")
endif(MINGW)

macro(xinx_automoc outfiles)
  qt4_get_moc_flags(moc_flags)
  qt4_extract_options(moc_files moc_options ${ARGN})

  foreach (it ${moc_files})
    get_filename_component(it ${it} ABSOLUTE)

    if ( EXISTS ${it} )
		file(READ ${it} _contents)

		string(REGEX MATCHALL "Q_OBJECT" _match "${_contents}")
		if(_match)
			qt4_make_output_file(${it} moc_ cxx outfile)
			qt4_create_moc_command(${it} ${outfile} "${moc_flags}" "${moc_options}")
			macro_add_file_dependencies(${it} ${outfile})
			set(${outfiles} ${${outfiles}} ${outfile})
		endif(_match)
    endif ( EXISTS ${it} )
  endforeach(it)
endmacro(xinx_automoc)

macro(initialisation_xinx)
	unset(LIBRARIES)

	include(${QT_USE_FILE})
	add_definitions(${QT_DEFINITIONS})

	if(${IS_PLUGINS})
		add_definitions(-DQT_PLUGIN)
		if(${IS_SHARED})
			add_definitions(-DQT_SHARED)
		else()
			add_definitions(-DQT_STATICPLUGIN)
		endif()
	endif()
	if(${USE_FRAMEWORK})
		set(MOC_FRAMEWORK -I${CMAKE_SOURCE_DIR}/framework -I${CMAKE_BINARY_DIR}/framework)
		include_directories(${CMAKE_SOURCE_DIR}/framework)
		include_directories(${CMAKE_BINARY_DIR}/framework)
		set(LIBRARIES ${LIBRARIES} framework)
	endif()
	if(${USE_COMPONENTS})
		include_directories(${CMAKE_SOURCE_DIR}/components)
		include_directories(${CMAKE_BINARY_DIR}/components)
		set(LIBRARIES ${LIBRARIES} xinxcmp)
	endif()
	if(${QCODEEDIT_FOUND})
		include_directories(${QCODEEDIT_INCLUDE_DIR})
		set(LIBRARIES ${LIBRARIES} ${QCODEEDIT_LIBRARY})
	endif()
	if(${IDEALITY_FOUND})
		include_directories(${IDEALITY_INCLUDE_DIR})
		set(LIBRARIES ${LIBRARIES} ${IDEALITY_LIBRARY})
	endif()
	if(${SVNCPP_FOUND})
		include_directories(${APR1_INCLUDE_DIR})
		include_directories(${SVNDEV_INCLUDE_DIR})
		include_directories(${SVNCPP_INCLUDE_DIR})
		set(LIBRARIES ${LIBRARIES} ${SVNCPP_LIBRARY} ${SVNDEV_LIBRARIES} ${APR1_LIBRARIES})
	endif()
	if(${LIBXML2_FOUND})
		add_definitions(${LIBXML2_DEFINITIONS})
		include_directories(${LIBXML2_INCLUDE_DIR})
		set(LIBRARIES ${LIBRARIES} ${LIBXML2_LIBRARIES})
	endif()
	if(${LIBXSLT_FOUND})
		add_definitions(${LIBXSLT_DEFINITIONS})
		include_directories(${LIBXSLT_INCLUDE_DIR})
		set(LIBRARIES ${LIBRARIES} ${LIBXSLT_LIBRARIES})
	endif()

	file(GLOB_RECURSE headers *.h   ${UNIQUE_APPPLICATION_HEADERS} ${SOAP_HEADERS})
	file(GLOB_RECURSE sources *.cpp ${UNIQUE_APPPLICATION_SOURCES} ${SOAP_SOURCES})
	file(GLOB_RECURSE forms *.ui)
	file(GLOB_RECURSE resources *.qrc)
	file(GLOB_RECURSE translations *.ts)

	if(WIN32)
		file(GLOB_RECURSE winresources *.rc)
	endif(WIN32)

	set_source_files_properties(${translations} PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/translations")


	qt4_wrap_ui(generated_forms ${forms})
	xinx_automoc(moc_headers ${headers} OPTIONS ${MOC_FRAMEWORK})
	qt4_add_resources(generated_resources ${resources})
	qt4_add_translation(translations_qm ${translations})
endmacro(initialisation_xinx)

macro(add_xinx_executable cible librairies)
	initialisation_xinx()

	add_executable(${cible} WIN32 ${moc_headers} ${sources} ${winresources} ${generated_forms} ${generated_resources} ${translations_qm})
	target_link_libraries(${cible} ${librairies} ${LIBRARIES} ${QT_LIBRARIES})

	install(TARGETS ${cible} DESTINATION bin)
endmacro(add_xinx_executable)

macro(add_xinx_librairie cible librairies version)
	initialisation_xinx()

	#if(WIN32)
	#	if(MINGW)
	#		set_target_properties(xinxcmp PROPERTIES LINK_FLAGS "-Wl,--enable-auto-import,--no-undefined,--enable-runtime-pseudo-reloc")
	#	endif(MINGW)
	#endif(WIN32)
	add_library(${cible} SHARED ${moc_headers} ${sources} ${winresources} ${generated_forms} ${generated_resources} ${translations_qm} )
	target_link_libraries(${cible} ${librairies} ${LIBRARIES} ${QT_LIBRARIES})
	set_target_properties(${cible} PROPERTIES VERSION ${version})
	if(UNIX)
		set_target_properties(${cible} PROPERTIES LINK_FLAGS "-rdynamic")
	endif(UNIX)

	install(TARGETS ${cible} DESTINATION bin)
endmacro(add_xinx_librairie)

macro(add_xinx_plugins cible is_shared librairies version)
	set(IS_PLUGINS True)
	set(IS_SHARED  ${is_shared})

	initialisation_xinx()

	if(${IS_SHARED})
		add_library(${cible} MODULE ${moc_headers} ${sources} ${winresources} ${generated_forms} ${generated_resources} ${translations_qm} )
	else()
		if(CMAKE_SIZEOF_VOID_P MATCHES 8)
			add_definitions(-fPIC)
		endif()
		add_library(${cible} STATIC ${moc_headers} ${sources} ${winresources} ${winresources}${generated_forms} ${generated_resources} ${translations_qm} )
	endif()

	target_link_libraries(${cible} ${librairies} ${LIBRARIES} ${QT_LIBRARIES})
	set_target_properties(${cible} PROPERTIES VERSION ${version})
	set_target_properties(${cible} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "../")
	if(UNIX)
		set_target_properties(${cible} PROPERTIES LINK_FLAGS "-rdynamic")
	endif(UNIX)

	install(TARGETS ${cible} DESTINATION plugins)
endmacro(add_xinx_plugins)
