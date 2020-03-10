#include "argtable3/argtable3.h"
#include "esp_console.h"
#include "esp_log.h"
#include "cmd_system.h"
#include "RadioConsole.hxx"
#include "vars.h"

/* non-class C style functions */
extern "C" {
static struct {
    struct arg_str *ssid;
    struct arg_str *passwd;
    struct arg_end *end;
} WiFIInfoArgs;

static int do_setWiFi_cmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&WiFIInfoArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, WiFIInfoArgs.end, argv[0]);
        return 0;
    }
    return 0;
}

static void registerWiFiInfo(void)
{
  WiFIInfoArgs.ssid = arg_str1("s", "ssid", "<ssid>", "Specify the ssld");
  WiFIInfoArgs.passwd = arg_str1("p", "password", "<password>", "Specify the password");
  WiFIInfoArgs.end = arg_end(2);
    const esp_console_cmd_t WiFi_cmd = {
        .command = "wifi",
        .help = "Set WiFi ssid and password.",
        .hint = NULL,
        .func = &do_setWiFi_cmd,
        .argtable = &WiFIInfoArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&WiFi_cmd));
}

static struct {
    struct arg_int *freq;
    struct arg_str *mode;
    struct arg_end *end;
} tuneArgs;

static int do_Tune_cmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&tuneArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, tuneArgs.end, argv[0]);
        return 0;
    }
    return 0;
}

static void registerTune(void)
{
  tuneArgs.freq = arg_int1("f", "freq", "<freq>", "Specify the freq");
  tuneArgs.mode = arg_str0("m", "mode", "<mode>", "Specify the moduration mode(AM, FM)");
  tuneArgs.end = arg_end(2);
    const esp_console_cmd_t Tune_cmd = {
        .command = "tune",
        .help = "Tune specified freq.",
        .hint = NULL,
        .func = &do_Tune_cmd,
        .argtable = &tuneArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&Tune_cmd));
}

static struct {
    struct arg_str *power;
    struct arg_end *end;
}  powerArgs;

static int do_power_cmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&powerArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, powerArgs.end, argv[0]);
        return 0;
    }
    return 0;
}

static void registerPower(void)
{
  powerArgs.power = arg_str1("s", "sw", "<sw>", "Specify On/Off");
  powerArgs.end = arg_end(1);
  const esp_console_cmd_t power_cmd = {
        .command = "power",
        .help = "Power switch on/off.",
        .hint = NULL,
        .func = &do_power_cmd,
        .argtable = &powerArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&power_cmd));
}

/*
static struct {
    struct arg_int *ssid;
    struct arg_int *passwd;
    struct arg_end *end;
}  WiFIInfoArgs;

static int do_setWiFi_cmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&WiFIInfoArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, WiFIInfoArgs.end, argv[0]);
        return 0;
    }
    return 0;
}

static void registerWiFiInfo(void)
{
  WiFIInfoArgs.ssid = arg_int1("s", "ssid", "<ssid>", "Specify the ssld");
  WiFIInfoArgs.passwd = arg_int1("p", "password", "<password>", "Specify the password");
    WiFIInfoArgs.end = arg_end(1);
    const esp_console_cmd_t WiFi_cmd = {
        .command = "wifi",
        .help = "Set WiFi ssid and password.",
        .hint = NULL,
        .func = &do_setWiFi_cmd,
        .argtable = &WiFIInfoArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&WiFi_cmd));
}
static struct {
    struct arg_int *ssid;
    struct arg_int *passwd;
    struct arg_end *end;
}  WiFIInfoArgs;

static int do_setWiFi_cmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&WiFIInfoArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, WiFIInfoArgs.end, argv[0]);
        return 0;
    }
    return 0;
}

static void registerWiFiInfo(void)
{
  WiFIInfoArgs.ssid = arg_int1("s", "ssid", "<ssid>", "Specify the ssld");
  WiFIInfoArgs.passwd = arg_int1("p", "password", "<password>", "Specify the password");
    WiFIInfoArgs.end = arg_end(1);
    const esp_console_cmd_t WiFi_cmd = {
        .command = "wifi",
        .help = "Set WiFi ssid and password.",
        .hint = NULL,
        .func = &do_setWiFi_cmd,
        .argtable = &WiFIInfoArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&WiFi_cmd));
}
static struct {
    struct arg_int *ssid;
    struct arg_int *passwd;
    struct arg_end *end;
}  WiFIInfoArgs;

static int do_setWiFi_cmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&WiFIInfoArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, WiFIInfoArgs.end, argv[0]);
        return 0;
    }
    return 0;
}

static void registerWiFiInfo(void)
{
  WiFIInfoArgs.ssid = arg_int1("s", "ssid", "<ssid>", "Specify the ssld");
  WiFIInfoArgs.passwd = arg_int1("p", "password", "<password>", "Specify the password");
    WiFIInfoArgs.end = arg_end(1);
    const esp_console_cmd_t WiFi_cmd = {
        .command = "wifi",
        .help = "Set WiFi ssid and password.",
        .hint = NULL,
        .func = &do_setWiFi_cmd,
        .argtable = &WiFIInfoArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&WiFi_cmd));
}
static struct {
    struct arg_int *ssid;
    struct arg_int *passwd;
    struct arg_end *end;
}  WiFIInfoArgs;

static int do_setWiFi_cmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&WiFIInfoArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, WiFIInfoArgs.end, argv[0]);
        return 0;
    }
    return 0;
}

static void registerWiFiInfo(void)
{
  WiFIInfoArgs.ssid = arg_int1("s", "ssid", "<ssid>", "Specify the ssld");
  WiFIInfoArgs.passwd = arg_int1("p", "password", "<password>", "Specify the password");
    WiFIInfoArgs.end = arg_end(1);
    const esp_console_cmd_t WiFi_cmd = {
        .command = "wifi",
        .help = "Set WiFi ssid and password.",
        .hint = NULL,
        .func = &do_setWiFi_cmd,
        .argtable = &WiFIInfoArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&WiFi_cmd));
}
static struct {
    struct arg_int *ssid;
    struct arg_int *passwd;
    struct arg_end *end;
}  WiFIInfoArgs;

static int do_setWiFi_cmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&WiFIInfoArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, WiFIInfoArgs.end, argv[0]);
        return 0;
    }
    return 0;
}

static void registerWiFiInfo(void)
{
  WiFIInfoArgs.ssid = arg_int1("s", "ssid", "<ssid>", "Specify the ssld");
  WiFIInfoArgs.passwd = arg_int1("p", "password", "<password>", "Specify the password");
    WiFIInfoArgs.end = arg_end(1);
    const esp_console_cmd_t WiFi_cmd = {
        .command = "wifi",
        .help = "Set WiFi ssid and password.",
        .hint = NULL,
        .func = &do_setWiFi_cmd,
        .argtable = &WiFIInfoArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&WiFi_cmd));
}
static struct {
    struct arg_int *ssid;
    struct arg_int *passwd;
    struct arg_end *end;
}  WiFIInfoArgs;

static int do_setWiFi_cmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&WiFIInfoArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, WiFIInfoArgs.end, argv[0]);
        return 0;
    }
    return 0;
}

static void registerWiFiInfo(void)
{
  WiFIInfoArgs.ssid = arg_int1("s", "ssid", "<ssid>", "Specify the ssld");
  WiFIInfoArgs.passwd = arg_int1("p", "password", "<password>", "Specify the password");
    WiFIInfoArgs.end = arg_end(1);
    const esp_console_cmd_t WiFi_cmd = {
        .command = "wifi",
        .help = "Set WiFi ssid and password.",
        .hint = NULL,
        .func = &do_setWiFi_cmd,
        .argtable = &WiFIInfoArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&WiFi_cmd));
}
static struct {
    struct arg_int *ssid;
    struct arg_int *passwd;
    struct arg_end *end;
}  WiFIInfoArgs;

static int do_setWiFi_cmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&WiFIInfoArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, WiFIInfoArgs.end, argv[0]);
        return 0;
    }
    return 0;
}

static void registerWiFiInfo(void)
{
  WiFIInfoArgs.ssid = arg_int1("s", "ssid", "<ssid>", "Specify the ssld");
  WiFIInfoArgs.passwd = arg_int1("p", "password", "<password>", "Specify the password");
    WiFIInfoArgs.end = arg_end(1);
    const esp_console_cmd_t WiFi_cmd = {
        .command = "wifi",
        .help = "Set WiFi ssid and password.",
        .hint = NULL,
        .func = &do_setWiFi_cmd,
        .argtable = &WiFIInfoArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&WiFi_cmd));
}
static struct {
    struct arg_int *ssid;
    struct arg_int *passwd;
    struct arg_end *end;
}  WiFIInfoArgs;

static int do_setWiFi_cmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&WiFIInfoArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, WiFIInfoArgs.end, argv[0]);
        return 0;
    }
    return 0;
}

static void registerWiFiInfo(void)
{
  WiFIInfoArgs.ssid = arg_int1("s", "ssid", "<ssid>", "Specify the ssld");
  WiFIInfoArgs.passwd = arg_int1("p", "password", "<password>", "Specify the password");
    WiFIInfoArgs.end = arg_end(1);
    const esp_console_cmd_t WiFi_cmd = {
        .command = "wifi",
        .help = "Set WiFi ssid and password.",
        .hint = NULL,
        .func = &do_setWiFi_cmd,
        .argtable = &WiFIInfoArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&WiFi_cmd));
}
*/
}

RadioConsole rcon;

int RadioConsole::init() {
  esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
  repl_config.prompt = "radio %";
  repl_config.history_save_path = HISTORY_PATH;
  ESP_ERROR_CHECK(esp_console_repl_init(&repl_config));
  register_system();
  register_cmd();
  ESP_ERROR_CHECK(esp_console_repl_start());
  return 0;
}
int RadioConsole::register_cmd() {
  registerWiFiInfo();
  registerTune();
  registerPower();
  return 0;
}