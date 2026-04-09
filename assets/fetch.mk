LIB_DIR := .
LIB_BASE_URL := https://github.com/Xaeian/AVR/wiki

.PHONY: fetch

fetch:
	@mkdir -p "$(LIB_DIR)"
	@set -e; \
	for name in $(filter-out $@,$(MAKECMDGOALS)); do \
	  curl -L -o "$(LIB_DIR)/$$name.c" "$(LIB_BASE_URL)/$$name.c"; \
	  curl -L -o "$(LIB_DIR)/$$name.h" "$(LIB_BASE_URL)/$$name.h"; \
	done

%:
	@: