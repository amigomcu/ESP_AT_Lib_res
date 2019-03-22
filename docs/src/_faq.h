/**
 * \page            page_faq Frequently asked questions
 * \tableofcontents
 *
 * \section         sect_faq_min_at_sw What is the minimum ESP8266 AT software version?
 *
 * Library follows latest AT releases available on official Espressif website.
 *
 *  - `1.7.0` AT command version is mandatory: https://www.espressif.com/en/products/software/esp-at/resource
 * 	- Use `AT+GMR\r\n` command to query current AT version running on ESP device
 *
 * \note            If you do not match these requirements, please update software on ESP to the latest one. Check \ref page_update_at_software section.
 *
 * \section         sect_faq_can_rtos Can I use this library with operating system?
 *
 * You may (and you <b>have to</b>) use this library with and only with operating system (or RTOS).
 * Library has advanced techniques to handle `AT` based software approach which is very optimized when used
 * with operating system and can optimize user application program layer.
 */