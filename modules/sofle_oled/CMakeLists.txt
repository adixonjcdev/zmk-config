if(CONFIG_SOFLE_OLED_LEFT)
  target_sources(app PRIVATE src/status_screen.c)
endif()

if(CONFIG_SOFLE_OLED_RIGHT)
  target_sources(app PRIVATE src/status_screen_right.c)
endif()