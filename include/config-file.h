/*
 * config-file.h
 *
 *  Created on: Sep 21, 2011
 *      Author: nick
 */

#ifndef CONFIG_FILE_H_
#define CONFIG_FILE_H_

/* config file boolean values */
#define CFG_VAL_ON "ON"
#define CFG_VAL_OFF "OFF"

int
parse_boolstr_value(const char *);

struct list_t *
parse_config(const char *);

#endif /* CONFIG_FILE_H_ */
