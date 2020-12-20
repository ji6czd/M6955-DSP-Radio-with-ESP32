idf_component_register(SRCS "src/Environ.cxx"
  REQUIRES json_cpp
                    INCLUDE_DIRS "include"
		    REQUIRES json nvs_flash fatfs Board)
