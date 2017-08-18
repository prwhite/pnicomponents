#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := pnicomponents

include $(IDF_PATH)/make/project.mk

clone: branch=$(shell git rev-parse --abbrev-ref HEAD)
clone: 
ifndef newname
	$(error Please supply new project directory name with command line argument "newname=<target directory>") 	
else
	@echo Cloning project to directory $(newname) from branch $(branch)
	mkdir -p $(newname)
	git archive $(branch) | tar -xC $(newname)/
	cd $(newname) ; sed -i '' 's/pnicomponents/$(notdir $(newname))/g' Makefile
	cd $(newname) ; git init ; git add . ; git commit -m "imported from template"
endif

