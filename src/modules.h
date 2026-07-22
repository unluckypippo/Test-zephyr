#ifndef APP_MODULES_H
#define APP_MODULES_H

#ifdef CONFIG_APP_MODULE_COUNTDOWN
void module_countdown_run(void);
#endif

#ifdef CONFIG_APP_MODULE_BLINK
void module_blink_run(void);
#endif

#ifdef CONFIG_APP_MODULE_BUTTON_I
void module_button_i_run(void);
#endif

#ifdef CONFIG_APP_MODULE_TEMP
void module_temp_read_run(void);
#endif

#endif /* APP_MODULES_H */
