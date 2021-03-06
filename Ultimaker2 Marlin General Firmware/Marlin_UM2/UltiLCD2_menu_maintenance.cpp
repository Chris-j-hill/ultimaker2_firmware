

#include "Configuration.h"
#ifdef ENABLE_ULTILCD2
#include "UltiLCD2.h"
#include "UltiLCD2_hi_lib.h"
#include "UltiLCD2_gfx.h"
#include "UltiLCD2_menu_maintenance.h"
#include "UltiLCD2_menu_first_run.h"
#include "UltiLCD2_menu_material.h"
#include "cardreader.h"
#include "lifetime_stats.h"
#include "ConfigurationStore.h"
#include "temperature.h"
#include "pins.h"


static void lcd_menu_maintenance_advanced();
static void lcd_menu_maintenance_advanced_heatup();
static void lcd_menu_maintenance_led();
static void lcd_menu_maintenance_extrude();
static void lcd_menu_maintenance_retraction();
static void lcd_menu_advanced_version();
static void lcd_menu_advanced_stats();
static void lcd_menu_maintenance_motion();
static void lcd_menu_advanced_factory_reset();
static void lcd_menu_maintenance_move_axis();

#ifdef MOVE_AXIS_MENU_OPTION_ENABLE
static void lcd_menu_maintenance_move_x_axis();
static void lcd_menu_maintenance_move_y_axis();
static void lcd_menu_maintenance_move_z_axis();
static void lcd_menu_maintenance_move_selected_axis();
#endif

#ifdef SAFER_EXTRUDE_MATERIAL_HEATUP
void check_for_extruder_temp_error();
uint8_t starting_temp;
int extrude_start_time;
#endif



void lcd_menu_maintenance()
{
  lcd_tripple_menu(PSTR("BUILD-|PLATE"), PSTR("ADVANCED"), PSTR("RETURN"));

  if (lcd_lib_button_pressed)
  {
    if (IS_SELECTED_MAIN(0))
      lcd_change_to_menu(lcd_menu_first_run_start_bed_leveling);
    else if (IS_SELECTED_MAIN(1))
      lcd_change_to_menu(lcd_menu_maintenance_advanced);
    else if (IS_SELECTED_MAIN(2))
      lcd_change_to_menu(lcd_menu_main);
  }

  lcd_lib_update_screen();
}

static char* lcd_advanced_item(uint8_t nr)
{
  int added_items = 0;

#ifdef DISABLE_STEPPER_MENU_OPTION_ENABLE
  added_items ++;
#endif

#ifdef MOVE_AXIS_MENU_OPTION_ENABLE
  added_items ++;
#endif

  if (nr == 0)
    strcpy_P(card.longFilename, PSTR("< RETURN"));
  else if (nr == 1)
    strcpy_P(card.longFilename, PSTR("LED settings"));
  else if (nr == 2)
#if EXTRUDERS < 2
    strcpy_P(card.longFilename, PSTR("Heatup nozzle"));
#else
    strcpy_P(card.longFilename, PSTR("Heatup first nozzle"));
  else if (nr == 3)
    strcpy_P(card.longFilename, PSTR("Heatup second nozzle"));
#endif
#if TEMP_SENSOR_BED != 0
  else if (nr == 2 + EXTRUDERS)
    strcpy_P(card.longFilename, PSTR("Heatup buildplate"));
#endif
  else if (nr == 2 + BED_MENU_OFFSET + EXTRUDERS)
    strcpy_P(card.longFilename, PSTR("Home head"));
  else if (nr == 3 + BED_MENU_OFFSET + EXTRUDERS)
    strcpy_P(card.longFilename, PSTR("Lower buildplate"));
  else if (nr == 4 + BED_MENU_OFFSET + EXTRUDERS)
    strcpy_P(card.longFilename, PSTR("Raise buildplate"));

#if EXTRUDERS >1
  else if (nr == 5 + BED_MENU_OFFSET + EXTRUDERS)
    strcpy_P(card.longFilename, PSTR("Insert material E1"));
  else if (nr == 6 + BED_MENU_OFFSET + EXTRUDERS)
    strcpy_P(card.longFilename, PSTR("Insert material E2"));
#else
  else if (nr == 5 + BED_MENU_OFFSET + EXTRUDERS)
    strcpy_P(card.longFilename, PSTR("Insert material"));
#endif

#ifdef DISABLE_STEPPER_MENU_OPTION_ENABLE
  else if (nr == 5 + BED_MENU_OFFSET + EXTRUDERS * 2)
    strcpy_P(card.longFilename, PSTR("Disable steppers"));
#endif

#ifdef MOVE_AXIS_MENU_OPTION_ENABLE
  else if (nr == 4 + added_items + BED_MENU_OFFSET + EXTRUDERS * 2)
    strcpy_P(card.longFilename, PSTR("Move Axis"));
#endif

  else if (nr == 5 +  added_items + BED_MENU_OFFSET + EXTRUDERS * 2)
#if EXTRUDERS < 2
    strcpy_P(card.longFilename, PSTR("Move material"));
#else
    strcpy_P(card.longFilename, PSTR("Move material (E1)"));
  else if (nr == 6 +  added_items + BED_MENU_OFFSET + EXTRUDERS * 2)
    strcpy_P(card.longFilename, PSTR("Move material (E2)"));
#endif
  else if (nr == 5 +  added_items + BED_MENU_OFFSET + EXTRUDERS * 3)
    strcpy_P(card.longFilename, PSTR("Set fan speed"));
  else if (nr == 6 +  added_items + BED_MENU_OFFSET + EXTRUDERS * 3)
    strcpy_P(card.longFilename, PSTR("Retraction settings"));
  else if (nr == 7 +  added_items + BED_MENU_OFFSET + EXTRUDERS * 3)
    strcpy_P(card.longFilename, PSTR("Motion settings"));
  else if (nr == 8 +  added_items + BED_MENU_OFFSET + EXTRUDERS * 3)
    strcpy_P(card.longFilename, PSTR("Version"));
  else if (nr == 9 +  added_items + BED_MENU_OFFSET + EXTRUDERS * 3)
    strcpy_P(card.longFilename, PSTR("Runtime stats"));
  else if (nr == 10 +  added_items + BED_MENU_OFFSET + EXTRUDERS * 3)
    strcpy_P(card.longFilename, PSTR("Factory reset"));
  else
    strcpy_P(card.longFilename, PSTR("???"));
  return card.longFilename;
}

static void lcd_advanced_details(uint8_t nr)
{
  char buffer[16];
  buffer[0] = '\0';

  int added_items = 0;
  //    int insert_e2= 0;

  //    #if EXTRUDER>1
  //    insert_e2 ++;
  //    #endif

#ifdef DISABLE_STEPPER_MENU_OPTION_ENABLE
  added_items ++;
#endif

#ifdef MOVE_AXIS_MENU_OPTION_ENABLE
  added_items ++;
#endif

  //    added_items += insert_e2;

  if (nr == 1)
  {
    int_to_string(led_brightness_level, buffer, PSTR("%"));
  } else if (nr == 2)
  {
    int_to_string(int(dsp_temperature[0]), buffer, PSTR("C/"));
    int_to_string(int(target_temperature[0]), buffer + strlen(buffer), PSTR("C"));
#if EXTRUDERS > 1
  } else if (nr == 3)
  {
    int_to_string(int(dsp_temperature[1]), buffer, PSTR("C/"));
    int_to_string(int(target_temperature[1]), buffer + strlen(buffer), PSTR("C"));
#endif
#if TEMP_SENSOR_BED != 0
  } else if (nr == 2 + EXTRUDERS)
  {
    int_to_string(int(dsp_temperature_bed), buffer, PSTR("C/"));
    int_to_string(int(target_temperature_bed), buffer + strlen(buffer), PSTR("C"));
#endif
  } else if (nr == 5 + added_items + BED_MENU_OFFSET + EXTRUDERS * 3)
  {
    int_to_string(int(fanSpeed) * 100 / 255, buffer, PSTR("%"));
  } else if (nr == 8 + added_items + BED_MENU_OFFSET + EXTRUDERS * 3)
  {
    lcd_lib_draw_stringP(5, 53, PSTR(STRING_CONFIG_H_AUTHOR));
    return;
  } else {
    return;
  }
  lcd_lib_draw_string(5, 53, buffer);
}

static void lcd_menu_maintenance_advanced_return()
{
  doCooldown();
  enquecommand_P(PSTR("G28 X0 Y0"));
  currentMenu = lcd_menu_maintenance_advanced;
}

static void lcd_menu_maintenance_advanced()
{
  int added_items = 0;

#ifdef DISABLE_STEPPER_MENU_OPTION_ENABLE
  added_items ++;
#endif

#ifdef MOVE_AXIS_MENU_OPTION_ENABLE
  added_items ++;
#endif

  lcd_scroll_menu(PSTR("ADVANCED"), 11 + BED_MENU_OFFSET + EXTRUDERS * 3 + added_items , lcd_advanced_item, lcd_advanced_details);

  if (lcd_lib_button_pressed)
  {
    if (IS_SELECTED_SCROLL(0))
      lcd_change_to_menu(lcd_menu_maintenance);
    else if (IS_SELECTED_SCROLL(1))
      lcd_change_to_menu(lcd_menu_maintenance_led, 0);
    else if (IS_SELECTED_SCROLL(2))
    {
      active_extruder = 0;
      lcd_change_to_menu(lcd_menu_maintenance_advanced_heatup, 0);
    }
#if EXTRUDERS > 1
    else if (IS_SELECTED_SCROLL(3))
    {
      active_extruder = 1;
      lcd_change_to_menu(lcd_menu_maintenance_advanced_heatup, 0);
    }
#endif
#if TEMP_SENSOR_BED != 0
    else if (IS_SELECTED_SCROLL(2 + EXTRUDERS))                     // Heatup buildplate
    {
      enquecommand_P(PSTR("G28 Z"));
      lcd_change_to_menu(lcd_menu_maintenance_advanced_bed_heatup, 0);
    }
#endif
    else if (IS_SELECTED_SCROLL(2 + BED_MENU_OFFSET + EXTRUDERS))   // Home head
    {
      lcd_lib_beep();
      enquecommand_P(PSTR("G28 X Y"));
      enquecommand_P(PSTR("M84"));        // Release motors
    }

    else if (IS_SELECTED_SCROLL(3 +  BED_MENU_OFFSET + EXTRUDERS))   // Lower bed
    {
      lcd_lib_beep();
      enquecommand_P(PSTR("G28 Z"));
      enquecommand_P(PSTR("M84"));        // Release motors
    }
    else if (IS_SELECTED_SCROLL(4 + BED_MENU_OFFSET + EXTRUDERS))   // Raise bed
    {
      char buffer[32];
      lcd_lib_beep();
      enquecommand_P(PSTR("G28 Z"));
      sprintf_P(buffer, PSTR("G1 F%i Z40"), int(homing_feedrate[Z_AXIS]));
      enquecommand(buffer);
      // Note: motors remain powered, otherwise the bed will descend by gravity.
    }

#if EXTRUDERS > 1
    else if (IS_SELECTED_SCROLL(5 + BED_MENU_OFFSET + EXTRUDERS))   // Insert E1
    {
      char buffer[32];
      enquecommand_P(PSTR("G28 X Y"));
      sprintf_P(buffer, PSTR("G1 F%i X%i Y%i"), int(homing_feedrate[X_AXIS]), int(X_MAX_LENGTH / 2), 10);
      enquecommand(buffer);
      active_extruder = 0;
      lcd_change_to_menu_insert_material(lcd_menu_maintenance_advanced_return);
    }

    else if (IS_SELECTED_SCROLL(6 + BED_MENU_OFFSET + EXTRUDERS))   // Insert E2
    {
      char buffer[32];
      enquecommand_P(PSTR("G28 X Y"));
      sprintf_P(buffer, PSTR("G1 F%i X%i Y%i"), int(homing_feedrate[X_AXIS]), int(X_MAX_LENGTH / 2), 10);
      enquecommand(buffer);
      active_extruder = 1;
      lcd_change_to_menu_insert_material(lcd_menu_maintenance_advanced_return);
    }

#else
    else if (IS_SELECTED_SCROLL(5 + BED_MENU_OFFSET + EXTRUDERS))   // Insert material
    {
      char buffer[32];
      enquecommand_P(PSTR("G28 X Y"));
      sprintf_P(buffer, PSTR("G1 F%i X%i Y%i"), int(homing_feedrate[X_AXIS]), int(X_MAX_LENGTH / 2), 10);
      enquecommand(buffer);
      lcd_change_to_menu_insert_material(lcd_menu_maintenance_advanced_return);
    }
#endif

#ifdef DISABLE_STEPPER_MENU_OPTION_ENABLE
    else if (IS_SELECTED_SCROLL(5 + BED_MENU_OFFSET + EXTRUDERS * 2)) //Disable steppers
    {
      lcd_lib_beep();
      enquecommand_P(PSTR("M84"));
    }
#endif

#ifdef MOVE_AXIS_MENU_OPTION_ENABLE   //if in config file
    else if (IS_SELECTED_SCROLL(4 + added_items + BED_MENU_OFFSET + EXTRUDERS * 2))  //Move axis
      lcd_change_to_menu(lcd_menu_maintenance_move_axis, 0);

#endif
    else if (IS_SELECTED_SCROLL(5 + added_items + BED_MENU_OFFSET + EXTRUDERS * 2)) // move material E1
    {
      set_extrude_min_temp(0);
      active_extruder = 0;
      target_temperature[active_extruder] = material[active_extruder].temperature;
#ifdef SAFER_EXTRUDE_MATERIAL_HEATUP
      starting_temp = current_temperature[active_extruder];
      extrude_start_time = millis();
#endif
      lcd_change_to_menu(lcd_menu_maintenance_extrude, 0);
    }
#if EXTRUDERS > 1
    else if (IS_SELECTED_SCROLL(6 + added_items + BED_MENU_OFFSET + EXTRUDERS * 2)) // move material E2
    {
      set_extrude_min_temp(0);
      active_extruder = 1;
      target_temperature[active_extruder] = material[active_extruder].temperature;
#ifdef SAFER_EXTRUDE_MATERIAL_HEATUP
      starting_temp = current_temperature[active_extruder];
      extrude_start_time = millis();
#endif
      lcd_change_to_menu(lcd_menu_maintenance_extrude, 0);
    }
#endif
    else if (IS_SELECTED_SCROLL(5 + added_items + BED_MENU_OFFSET + EXTRUDERS * 3))
      LCD_EDIT_SETTING_BYTE_PERCENT(fanSpeed, "Fan speed", "%", 0, 100);
    else if (IS_SELECTED_SCROLL(6 + added_items + BED_MENU_OFFSET + EXTRUDERS * 3))
      lcd_change_to_menu(lcd_menu_maintenance_retraction, SCROLL_MENU_ITEM_POS(0));
    else if (IS_SELECTED_SCROLL(7 + added_items + BED_MENU_OFFSET + EXTRUDERS * 3))
      lcd_change_to_menu(lcd_menu_maintenance_motion, SCROLL_MENU_ITEM_POS(0));
    else if (IS_SELECTED_SCROLL(8 + added_items + BED_MENU_OFFSET + EXTRUDERS * 3))
      lcd_change_to_menu(lcd_menu_advanced_version, SCROLL_MENU_ITEM_POS(0));
    else if (IS_SELECTED_SCROLL(9 + added_items + BED_MENU_OFFSET + EXTRUDERS * 3))
      lcd_change_to_menu(lcd_menu_advanced_stats, SCROLL_MENU_ITEM_POS(0));
    else if (IS_SELECTED_SCROLL(10 + added_items + BED_MENU_OFFSET + EXTRUDERS * 3))
      lcd_change_to_menu(lcd_menu_advanced_factory_reset, SCROLL_MENU_ITEM_POS(1));
  }
}

static void lcd_menu_maintenance_advanced_heatup()
{
  if (lcd_lib_encoder_pos / ENCODER_TICKS_PER_SCROLL_MENU_ITEM != 0)
  {
    target_temperature[active_extruder] = int(target_temperature[active_extruder]) + (lcd_lib_encoder_pos / ENCODER_TICKS_PER_SCROLL_MENU_ITEM);
    if (target_temperature[active_extruder] < 0)
      target_temperature[active_extruder] = 0;
    if (target_temperature[active_extruder] > HEATER_0_MAXTEMP - 15)
      target_temperature[active_extruder] = HEATER_0_MAXTEMP - 15;
    lcd_lib_encoder_pos = 0;
  }
  if (lcd_lib_button_pressed)
    lcd_change_to_menu(previousMenu, previousEncoderPos);

  lcd_lib_clear();
  lcd_lib_draw_string_centerP(20, PSTR("Nozzle temperature:"));
  lcd_lib_draw_string_centerP(53, PSTR("Click to return"));
  char buffer[16];
  int_to_string(int(dsp_temperature[active_extruder]), buffer, PSTR("C/"));
  int_to_string(int(target_temperature[active_extruder]), buffer + strlen(buffer), PSTR("C"));
  lcd_lib_draw_string_center(30, buffer);
  lcd_lib_update_screen();
}

void lcd_menu_maintenance_extrude()
{
  if (lcd_lib_encoder_pos / ENCODER_TICKS_PER_SCROLL_MENU_ITEM != 0)
  {
    if (printing_state == PRINT_STATE_NORMAL && movesplanned() < 3)
    {
      current_position[E_AXIS] += lcd_lib_encoder_pos * 0.1;
      plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 10, active_extruder);
      lcd_lib_encoder_pos = 0;
    }
  }
  if (lcd_lib_button_pressed)
  {
    set_extrude_min_temp(EXTRUDE_MINTEMP);
    target_temperature[active_extruder] = 0;
    lcd_change_to_menu(previousMenu, previousEncoderPos);
  }
#ifdef SAFER_EXTRUDE_MATERIAL_HEATUP
  check_for_extruder_temp_error();    // check for temp error if not heating, eg if heater cartridge not connected properly
  // Note: will not detect broken/disconnected sensors, assume that was caught on startup
#endif
  lcd_lib_clear();
  lcd_lib_draw_string_centerP(20, PSTR("Nozzle temperature:"));
  lcd_lib_draw_string_centerP(40, PSTR("Rotate to extrude"));
  lcd_lib_draw_string_centerP(53, PSTR("Click to return"));
  char buffer[16];
  int_to_string(int(dsp_temperature[active_extruder]), buffer, PSTR("C/"));
  int_to_string(int(target_temperature[active_extruder]), buffer + strlen(buffer), PSTR("C"));
  lcd_lib_draw_string_center(30, buffer);
  lcd_lib_update_screen();
}

#if TEMP_SENSOR_BED != 0
void lcd_menu_maintenance_advanced_bed_heatup()
{
  if (lcd_lib_encoder_pos / ENCODER_TICKS_PER_SCROLL_MENU_ITEM != 0)
  {
    target_temperature_bed = int(target_temperature_bed) + (lcd_lib_encoder_pos / ENCODER_TICKS_PER_SCROLL_MENU_ITEM);
    if (target_temperature_bed < 0)
      target_temperature_bed = 0;
    if (target_temperature_bed > BED_MAXTEMP - 15)
      target_temperature_bed = BED_MAXTEMP - 15;
    lcd_lib_encoder_pos = 0;
  }
  if (lcd_lib_button_pressed)
    lcd_change_to_menu(previousMenu, previousEncoderPos);

  lcd_lib_clear();
  lcd_lib_draw_string_centerP(20, PSTR("Buildplate temp.:"));
  lcd_lib_draw_string_centerP(53, PSTR("Click to return"));
  char buffer[16];
  int_to_string(int(dsp_temperature_bed), buffer, PSTR("C/"));
  int_to_string(int(target_temperature_bed), buffer + strlen(buffer), PSTR("C"));
  lcd_lib_draw_string_center(30, buffer);
  lcd_lib_update_screen();
}
#endif

//#ifdef SAFER_EXTRUDE_MATERIAL_HEATUP
//#define SMALL_TEMP_INCREMENT 5        // assume if properly connected can heat up this amount
//#define SHORT_TIME_PERIOD 10000       // in this time period
//#define REASONABLY_LOW_TEMP 100       // provided the temperature is below this (Note: rate of temp increase slows at higher temps)
//#define MIN_FROM_TARGET_TEMP 20
//#endif

#ifdef SAFER_EXTRUDE_MATERIAL_HEATUP
void check_for_extruder_temp_error() {
  //assume heater able to heat up by small increment in short time if properly connected and reading relatively cold temp
  if ((current_temperature[active_extruder] < REASONABLY_LOW_TEMP) && (current_temperature[active_extruder] < starting_temp + SMALL_TEMP_INCREMENT) && (millis() > extrude_start_time + SHORT_TIME_PERIOD)) {
    if (target_temperature[active_extruder] >= starting_temp + MIN_FROM_TARGET_TEMP) {
      disable_heater();
      Stop(STOP_REASON_HEATER_ERROR);
    }
    else if ((target_temperature[active_extruder] < starting_temp + MIN_FROM_TARGET_TEMP) && (millis() > extrude_start_time + SHORT_TIME_PERIOD * 3)) { //longer time window if target temp near start temp
      disable_heater();
      Stop(STOP_REASON_HEATER_ERROR);
    }
  }
}
#endif
void lcd_menu_advanced_version()
{
  lcd_info_screen(previousMenu, NULL, PSTR("Return"));
  lcd_lib_draw_string_centerP(30, PSTR(STRING_VERSION_CONFIG_H));
  lcd_lib_draw_string_centerP(40, PSTR(STRING_CONFIG_H_AUTHOR));
  lcd_lib_update_screen();
}

void lcd_menu_advanced_stats()
{
  lcd_info_screen(previousMenu, NULL, PSTR("Return"));
  lcd_lib_draw_string_centerP(10, PSTR("Machine on for:"));
  char buffer[LCD_MAX_TEXT_LINE_LENGTH + 1];      // Longest string = "hh:mm Mat:12345678m" = 19 characters + 1
  char* c = int_to_string(lifetime_minutes / 60, buffer, PSTR(":"));
  if (lifetime_minutes % 60 < 10)
    *c++ = '0';
  c = int_to_string(lifetime_minutes % 60, c);
  lcd_lib_draw_string_center(20, buffer);

  lcd_lib_draw_string_centerP(30, PSTR("Printing:"));
  c = int_to_string(lifetime_print_minutes / 60, buffer, PSTR(":"));
  if (lifetime_print_minutes % 60 < 10)
    *c++ = '0';
  c = int_to_string(lifetime_print_minutes % 60, c);
  strcpy_P(c, PSTR(" Mat:"));
  c += 5;
  c = int_to_string(lifetime_print_centimeters / 100, c, PSTR("m"));
  lcd_lib_draw_string_center(40, buffer);
  lcd_lib_update_screen();
}

static void doFactoryReset()
{
  //Clear the EEPROM settings so they get read from default.
  eeprom_write_byte((uint8_t*)100, 0);
  eeprom_write_byte((uint8_t*)101, 0);
  eeprom_write_byte((uint8_t*)102, 0);
  eeprom_write_byte((uint8_t*)EEPROM_FIRST_RUN_DONE_OFFSET, 0);
  eeprom_write_byte(EEPROM_MATERIAL_COUNT_OFFSET(), 0);

  cli();
  //NOTE: Jumping to address 0 is not a fully proper way to reset.
  // Letting the watchdog timeout is a better reset, but the bootloader does not continue on a watchdog timeout.
  // So we disable interrupts and hope for the best!
  //Jump to address 0x0000
#ifdef __AVR__
  asm volatile(
    "clr  r30   \n\t"
    "clr  r31   \n\t"
    "ijmp \n\t"
  );
#else
  //TODO
#endif
}

static void lcd_menu_advanced_factory_reset()
{
  lcd_question_screen(NULL, doFactoryReset, PSTR("YES"), previousMenu, NULL, PSTR("NO"));

  lcd_lib_draw_string_centerP(10, PSTR("Reset everything"));
  lcd_lib_draw_string_centerP(20, PSTR("to default?"));
  lcd_lib_update_screen();
}


static char* lcd_retraction_item(uint8_t nr)
{
  if (nr == 0)
    strcpy_P(card.longFilename, PSTR("< RETURN"));
  else if (nr == 1)
    strcpy_P(card.longFilename, PSTR("Retract length"));
  else if (nr == 2)
    strcpy_P(card.longFilename, PSTR("Retract speed"));
  else
    strcpy_P(card.longFilename, PSTR("???"));
  return card.longFilename;
}

static void lcd_retraction_details(uint8_t nr)
{
  char buffer[16];
  if (nr == 0)
    return;
  else if (nr == 1)
    float_to_string(retract_length, buffer, PSTR("mm"));
  else if (nr == 2)
    int_to_string(retract_feedrate / 60 + 0.5, buffer, PSTR("mm/sec"));
  lcd_lib_draw_string(5, 53, buffer);
}

static void lcd_menu_maintenance_retraction()
{
  lcd_scroll_menu(PSTR("RETRACTION"), 3, lcd_retraction_item, lcd_retraction_details);
  if (lcd_lib_button_pressed)
  {
    if (IS_SELECTED_SCROLL(0))
    {
      Config_StoreSettings();
      lcd_change_to_menu(lcd_menu_maintenance_advanced, SCROLL_MENU_ITEM_POS(6 + BED_MENU_OFFSET + EXTRUDERS * 2));
    }
    else if (IS_SELECTED_SCROLL(1))
      LCD_EDIT_SETTING_FLOAT001(retract_length, "Retract length", "mm", 0, 50);
    else if (IS_SELECTED_SCROLL(2))
      LCD_EDIT_SETTING_SPEED(retract_feedrate, "Retract speed", "mm/sec", 0, max_feedrate[E_AXIS] * 60);
  }
}

static char* lcd_motion_item(uint8_t nr)
{
  if (nr == 0)
    strcpy_P(card.longFilename, PSTR("< RETURN"));
  else if (nr == 1)
    strcpy_P(card.longFilename, PSTR("Acceleration"));
  else if (nr == 2)
    strcpy_P(card.longFilename, PSTR("X/Y Jerk"));
  else if (nr == 3)
    strcpy_P(card.longFilename, PSTR("Max speed X"));
  else if (nr == 4)
    strcpy_P(card.longFilename, PSTR("Max speed Y"));
  else if (nr == 5)
    strcpy_P(card.longFilename, PSTR("Max speed Z"));
  else if (nr == 6)
    strcpy_P(card.longFilename, PSTR("Current X/Y"));
  else if (nr == 7)
    strcpy_P(card.longFilename, PSTR("Current Z"));
  else if (nr == 8)
    strcpy_P(card.longFilename, PSTR("Current E"));
  else
    strcpy_P(card.longFilename, PSTR("???"));
  return card.longFilename;
}

static void lcd_motion_details(uint8_t nr)
{
  char buffer[16];
  if (nr == 0)
    return;
  else if (nr == 1)
    int_to_string(acceleration, buffer, PSTR("mm/sec^2"));
  else if (nr == 2)
    int_to_string(max_xy_jerk, buffer, PSTR("mm/sec"));
  else if (nr == 3)
    int_to_string(max_feedrate[X_AXIS], buffer, PSTR("mm/sec"));
  else if (nr == 4)
    int_to_string(max_feedrate[Y_AXIS], buffer, PSTR("mm/sec"));
  else if (nr == 5)
    int_to_string(max_feedrate[Z_AXIS], buffer, PSTR("mm/sec"));
  else if (nr == 6)
    int_to_string(motor_current_setting[0], buffer, PSTR("mA"));
  else if (nr == 7)
    int_to_string(motor_current_setting[1], buffer, PSTR("mA"));
  else if (nr == 8)
    int_to_string(motor_current_setting[2], buffer, PSTR("mA"));
  lcd_lib_draw_string(5, 53, buffer);
}

static void lcd_menu_maintenance_motion()
{
  lcd_scroll_menu(PSTR("MOTION"), 9, lcd_motion_item, lcd_motion_details);
  if (lcd_lib_button_pressed)
  {
    if (IS_SELECTED_SCROLL(0))
    {
      digipot_current(0, motor_current_setting[0]);
      digipot_current(1, motor_current_setting[1]);
      digipot_current(2, motor_current_setting[2]);
      Config_StoreSettings();
      lcd_change_to_menu(lcd_menu_maintenance_advanced, SCROLL_MENU_ITEM_POS(7 + BED_MENU_OFFSET + EXTRUDERS * 2));
    }
    else if (IS_SELECTED_SCROLL(1))
      LCD_EDIT_SETTING_FLOAT100(acceleration, "Acceleration", "mm/sec^2", 0, 20000);
    else if (IS_SELECTED_SCROLL(2))
      LCD_EDIT_SETTING_FLOAT1(max_xy_jerk, "X/Y Jerk", "mm/sec", 0, 100);
    else if (IS_SELECTED_SCROLL(3))
      LCD_EDIT_SETTING_FLOAT1(max_feedrate[X_AXIS], "Max speed X", "mm/sec", 0, 1000);
    else if (IS_SELECTED_SCROLL(4))
      LCD_EDIT_SETTING_FLOAT1(max_feedrate[Y_AXIS], "Max speed Y", "mm/sec", 0, 1000);
    else if (IS_SELECTED_SCROLL(5))
      LCD_EDIT_SETTING_FLOAT1(max_feedrate[Z_AXIS], "Max speed Z", "mm/sec", 0, 1000);
    else if (IS_SELECTED_SCROLL(6))
      LCD_EDIT_SETTING(motor_current_setting[0], "Current X/Y", "mA", 0, 1300);
    else if (IS_SELECTED_SCROLL(7))
      LCD_EDIT_SETTING(motor_current_setting[1], "Current Z", "mA", 0, 1300);
    else if (IS_SELECTED_SCROLL(8))
      LCD_EDIT_SETTING(motor_current_setting[2], "Current E", "mA", 0, 1300);
  }
}

static char* lcd_led_item(uint8_t nr)
{
  if (nr == 0)
    strcpy_P(card.longFilename, PSTR("< RETURN"));
  else if (nr == 1)
    strcpy_P(card.longFilename, PSTR("Brightness"));
  else if (nr == 2)
    strcpy_P(card.longFilename, PSTR(" Always On"));
  else if (nr == 3)
    strcpy_P(card.longFilename, PSTR(" Always Off"));
  else if (nr == 4)
    strcpy_P(card.longFilename, PSTR(" On while printing"));
  else if (nr == 5)
    strcpy_P(card.longFilename, PSTR(" Glow when done"));
  else
    strcpy_P(card.longFilename, PSTR("???"));
  if (nr - 2 == led_mode)
    card.longFilename[0] = '>';
  return card.longFilename;
}

static void lcd_led_details(uint8_t nr)
{
  char buffer[16];
  if (nr == 0)
    return;
  else if (nr == 1)
  {
    int_to_string(led_brightness_level, buffer, PSTR("%"));
    lcd_lib_draw_string(5, 53, buffer);
  }
}

static void lcd_menu_maintenance_led()
{
  analogWrite(LED_PIN, 255 * int(led_brightness_level) / 100);
  lcd_scroll_menu(PSTR("LED"), 6, lcd_led_item, lcd_led_details);
  if (lcd_lib_button_pressed)
  {
    if (IS_SELECTED_SCROLL(0))
    {
      if (led_mode != LED_MODE_ALWAYS_ON)
        analogWrite(LED_PIN, 0);
      Config_StoreSettings();
      lcd_change_to_menu(lcd_menu_maintenance_advanced, SCROLL_MENU_ITEM_POS(1));
    }
    else if (IS_SELECTED_SCROLL(1))
    {
      LCD_EDIT_SETTING(led_brightness_level, "Brightness", "%", 0, 100);
    }
    else if (IS_SELECTED_SCROLL(2))
    {
      led_mode = LED_MODE_ALWAYS_ON;
      lcd_lib_beep();
    }
    else if (IS_SELECTED_SCROLL(3))
    {
      led_mode = LED_MODE_ALWAYS_OFF;
      lcd_lib_beep();
    }
    else if (IS_SELECTED_SCROLL(4))
    {
      led_mode = LED_MODE_WHILE_PRINTING;
      lcd_lib_beep();
    }
    else if (IS_SELECTED_SCROLL(5))
    {
      led_mode = LED_MODE_BLINK_ON_DONE;
      lcd_lib_beep();
    }
  }
}

#ifdef MOVE_AXIS_MENU_OPTION_ENABLE

static char* lcd_advanced_move_axis_item(uint8_t nr)
{
  if (nr == 0)
    strcpy_P(card.longFilename, PSTR("< RETURN"));
  else if (nr == 1)
    strcpy_P(card.longFilename, PSTR("X Axis"));
  else if (nr == 2)
    strcpy_P(card.longFilename, PSTR("Y Axis"));
  else if (nr == 3)
    strcpy_P(card.longFilename, PSTR("Z Axis"));

#if EXTRUDERS >1
  else if (nr == 4)
    strcpy_P(card.longFilename, PSTR("E1 Axis"));
  else if (nr == 5)
    strcpy_P(card.longFilename, PSTR("E2 Axis"));
  else if (nr == 6)
    strcpy_P(card.longFilename, PSTR("Home Axis"));
#else
  else if (nr == 4)
    strcpy_P(card.longFilename, PSTR("E Axis"));
  else if (nr == 5)
    strcpy_P(card.longFilename, PSTR("Home Axis"));
#endif
  else
    strcpy_P(card.longFilename, PSTR("???"));
  return card.longFilename;
}

static void lcd_advanced_move_axis_details(uint8_t nr)
{
  char buffer[16];
  buffer[0] = '\0';
  if (nr == 1)
  {
    int_to_string(current_position[X_AXIS], buffer, PSTR("mm"));
  }
  else if (nr == 2)
  {
    int_to_string(current_position[Y_AXIS], buffer, PSTR("mm"));
  }
  else if (nr == 3)
  {
    int_to_string(current_position[Z_AXIS], buffer, PSTR("mm"));
  }
  else if (nr == 4)
  {
    active_extruder = 0;
    int_to_string(current_position[E_AXIS], buffer, PSTR("mm"));
  }
#if EXTRUDERS > 1
  else if (nr == 5)
  {
    active_extruder = 1;
    int_to_string(current_position[E_AXIS], buffer, PSTR("mm"));
  }
#endif
  else {
    return;
  }
  lcd_lib_draw_string(5, 53, buffer);
}

int selected_axis = 0;

static void lcd_menu_maintenance_move_x_axis() {   //select axis to move
  selected_axis = 1;
  lcd_change_to_menu(lcd_menu_maintenance_move_selected_axis, 0);
}
static void lcd_menu_maintenance_move_y_axis() {
  selected_axis = 2;
  lcd_change_to_menu(lcd_menu_maintenance_move_selected_axis, 0);
}

static void lcd_menu_maintenance_move_z_axis() {
  selected_axis = 3;
  lcd_change_to_menu(lcd_menu_maintenance_move_selected_axis, 0);
}

char prev_axis_buffer[8];    //issue of buffer not being written every cycle, have display one and one to hold previous in case where encoder not moved
static void lcd_menu_maintenance_move_selected_axis()    //move selected axis with encoder
{

  char buffer[8];
  lcd_lib_clear();
  if (lcd_lib_encoder_pos / ENCODER_TICKS_PER_SCROLL_MENU_ITEM != 0)
  {

    if (printing_state == PRINT_STATE_NORMAL && movesplanned() < 3)
    {
      if (selected_axis == 1)
      {
        current_position[X_AXIS] += lcd_lib_encoder_pos * 0.5;
        if (current_position[X_AXIS] < X_MIN_POS) current_position[X_AXIS] = X_MIN_POS;
        else if (current_position[X_AXIS] > X_MAX_POS) current_position[X_AXIS] = X_MAX_POS;
        float_to_string(current_position[X_AXIS], buffer, PSTR("mm"));
        //              lcd_lib_draw_string_center(30, buffer);
      }
      else if (selected_axis == 2)
      {
        current_position[Y_AXIS] += lcd_lib_encoder_pos * 0.5;
        if (current_position[Y_AXIS] < Y_MIN_POS) current_position[Y_AXIS] = Y_MIN_POS;
        else if (current_position[Y_AXIS] > Y_MAX_POS) current_position[Y_AXIS] = Y_MAX_POS;
        float_to_string(current_position[Y_AXIS], buffer, PSTR("mm"));
        //              lcd_lib_draw_string_center(30, buffer);
      }
      else if (selected_axis == 3)
      {
        current_position[Z_AXIS] += lcd_lib_encoder_pos * 0.25;
        if (current_position[Z_AXIS] < Z_MIN_POS) current_position[Z_AXIS] = Z_MIN_POS;
        else if (current_position[Z_AXIS] > Z_MAX_POS) current_position[Z_AXIS] = Z_MAX_POS;
        float_to_string(current_position[Z_AXIS], buffer, PSTR("mm"));

      }
      plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 50, active_extruder);
      lcd_lib_encoder_pos = 0;

    }
  }

  else
  {
    for (int alpha = 0; alpha < sizeof(buffer); alpha++) {
      buffer[alpha] = prev_axis_buffer[alpha];
    }
  }

  if (lcd_lib_button_pressed)
  {
    lcd_change_to_menu(lcd_menu_maintenance_move_axis, 0);
  }

  lcd_lib_draw_string_centerP(20, PSTR("Position:"));
  lcd_lib_draw_string_centerP(40, PSTR("Rotate to Move"));
  lcd_lib_draw_string_centerP(53, PSTR("Click to return"));
  lcd_lib_draw_string_center(30, buffer);
  for (int alpha = 0; alpha < sizeof(buffer); alpha++) {
    prev_axis_buffer[alpha] = buffer[alpha];
  }
  lcd_lib_update_screen();
}


static void lcd_menu_maintenance_move_axis()
{
  lcd_scroll_menu(PSTR("MOVE AXIS"), 5 + EXTRUDERS, lcd_advanced_move_axis_item, lcd_advanced_move_axis_details);

  if (lcd_lib_button_pressed)
  {
    if (IS_SELECTED_SCROLL(0))
      lcd_change_to_menu(lcd_menu_maintenance_advanced);
    else if (IS_SELECTED_SCROLL(1))
      lcd_change_to_menu(lcd_menu_maintenance_move_x_axis, 0);
    else if (IS_SELECTED_SCROLL(2))
      lcd_change_to_menu(lcd_menu_maintenance_move_y_axis, 0);
    else if (IS_SELECTED_SCROLL(3))
      lcd_change_to_menu(lcd_menu_maintenance_move_z_axis, 0);

    else if (IS_SELECTED_SCROLL(4))
    {
      set_extrude_min_temp(0);
      active_extruder = 0;
      target_temperature[active_extruder] = material[active_extruder].temperature;
#ifdef SAFER_EXTRUDE_MATERIAL_HEATUP
      starting_temp = current_temperature[active_extruder];
      extrude_start_time = millis();
#endif
      lcd_change_to_menu(lcd_menu_maintenance_extrude, 0);
    }

#if EXTRUDERS > 1
    else if (IS_SELECTED_SCROLL(5))
    {
      set_extrude_min_temp(0);
      active_extruder = 1;
      target_temperature[active_extruder] = material[active_extruder].temperature;
#ifdef SAFER_EXTRUDE_MATERIAL_HEATUP
      starting_temp = current_temperature[active_extruder];
      extrude_start_time = millis();
#endif
      lcd_change_to_menu(lcd_menu_maintenance_extrude, 0);
    }
#endif

    else if (IS_SELECTED_SCROLL(4 + EXTRUDERS))   // Home head
    {
      lcd_lib_beep();
      enquecommand_P(PSTR("G28 X Y Z"));
      enquecommand_P(PSTR("M84"));
    }
  }
}

#endif

static void passcode_details(int nr) {
}

static void passcode_items(int nr) {

  if (nr == 0)
    strcpy_P(card.longFilename, PSTR("0"));
  else if (nr == 1)
    strcpy_P(card.longFilename, PSTR("1"));
  else if (nr == 2)
    strcpy_P(card.longFilename, PSTR("2"));



}


#endif//ENABLE_ULTILCD2
