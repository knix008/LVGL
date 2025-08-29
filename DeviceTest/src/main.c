#include "camera.h"
#include "network.h"
#include "serial.h"
#include "wiegand.h"
#include "lcd.h"
#include "cpu.h"
#include "emmc.h"
#include "speaker.h"
#include "led.h"
#include "bluetooth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to run automated test suite
void run_automated_tests(int camera_index, const char* network_interface, const char* serial_device, int serial_baud_rate, int wiegand_data0_pin, int wiegand_data1_pin) {
    printf("=== Automated Device Test Suite ===\n");
    printf("Starting comprehensive device testing...\n");
    
    test_summary_t camera_summary = run_all_camera_tests(camera_index);
    test_summary_t network_summary = run_all_network_tests(network_interface);
    test_summary_t serial_summary = {0, 0, 0, 0.0, ""};
    test_summary_t wiegand_summary = {0, 0, 0, 0.0, ""};
    test_summary_t lcd_summary = {0, 0, 0, 0.0, ""};
    test_summary_t cpu_summary = {0, 0, 0, 0.0, ""};
    test_summary_t emmc_summary = {0, 0, 0, 0.0, ""};
    test_summary_t speaker_summary = {0, 0, 0, 0.0, ""};
    test_summary_t led_summary = {0, 0, 0, 0.0, ""};
    test_summary_t bluetooth_summary = {0, 0, 0, 0.0, ""};
    
    // Run serial tests if device is specified
    if (serial_device && strlen(serial_device) > 0) {
        serial_summary = run_all_serial_tests(serial_device, serial_baud_rate);
    }
    
    // Run Wiegand tests if pins are specified
    if (wiegand_data0_pin >= 0 && wiegand_data1_pin >= 0) {
        wiegand_summary = run_all_wiegand_tests(wiegand_data0_pin, wiegand_data1_pin);
    }
    
    // Run LCD tests (try different interfaces)
    lcd_summary = run_all_lcd_tests(LCD_INTERFACE_HDMI, NULL);
    
    // Run CPU tests
    cpu_summary = run_all_cpu_tests();
    
    // Run eMMC tests (try common device paths)
    emmc_summary = run_all_emmc_tests("/dev/mmcblk0");
    
    // Run speaker tests
    speaker_summary = run_all_speaker_tests(NULL);
    
    // Run LED tests (try common GPIO pins)
    led_summary = run_all_led_tests(17, 18);
    
    // Run Bluetooth tests
    bluetooth_summary = run_all_bluetooth_tests();
    
    // Overall summary
    printf("\n=== TEST SUMMARY ===\n");
    printf("Camera Tests: %s\n", camera_summary.summary);
    printf("Network Tests: %s\n", network_summary.summary);
    if (serial_device && strlen(serial_device) > 0) {
        printf("Serial Tests: %s\n", serial_summary.summary);
    }
    if (wiegand_data0_pin >= 0 && wiegand_data1_pin >= 0) {
        printf("Wiegand Tests: %s\n", wiegand_summary.summary);
    }
    printf("LCD Tests: %s\n", lcd_summary.summary);
    printf("CPU Tests: %s\n", cpu_summary.summary);
    printf("eMMC Tests: %s\n", emmc_summary.summary);
    printf("Speaker Tests: %s\n", speaker_summary.summary);
    printf("LED Tests: %s\n", led_summary.summary);
    printf("Bluetooth Tests: %s\n", bluetooth_summary.summary);
    
    int total_tests = camera_summary.total_tests + network_summary.total_tests + serial_summary.total_tests + wiegand_summary.total_tests + lcd_summary.total_tests + cpu_summary.total_tests + emmc_summary.total_tests + speaker_summary.total_tests + led_summary.total_tests + bluetooth_summary.total_tests;
    int total_passed = camera_summary.passed_tests + network_summary.passed_tests + serial_summary.passed_tests + wiegand_summary.passed_tests + lcd_summary.passed_tests + cpu_summary.passed_tests + emmc_summary.passed_tests + speaker_summary.passed_tests + led_summary.passed_tests + bluetooth_summary.passed_tests;
    int total_failed = camera_summary.failed_tests + network_summary.failed_tests + serial_summary.failed_tests + wiegand_summary.failed_tests + lcd_summary.failed_tests + cpu_summary.failed_tests + emmc_summary.failed_tests + speaker_summary.failed_tests + led_summary.failed_tests + bluetooth_summary.failed_tests;
    double overall_score = 0.0;
    int score_count = 0;
    
    if (camera_summary.total_tests > 0) {
        overall_score += camera_summary.average_score;
        score_count++;
    }
    if (network_summary.total_tests > 0) {
        overall_score += network_summary.average_score;
        score_count++;
    }
    if (serial_summary.total_tests > 0) {
        overall_score += serial_summary.average_score;
        score_count++;
    }
    if (wiegand_summary.total_tests > 0) {
        overall_score += wiegand_summary.average_score;
        score_count++;
    }
    if (lcd_summary.total_tests > 0) {
        overall_score += lcd_summary.average_score;
        score_count++;
    }
    if (cpu_summary.total_tests > 0) {
        overall_score += cpu_summary.average_score;
        score_count++;
    }
    if (emmc_summary.total_tests > 0) {
        overall_score += emmc_summary.average_score;
        score_count++;
    }
    if (speaker_summary.total_tests > 0) {
        overall_score += speaker_summary.average_score;
        score_count++;
    }
    if (led_summary.total_tests > 0) {
        overall_score += led_summary.average_score;
        score_count++;
    }
    if (bluetooth_summary.total_tests > 0) {
        overall_score += bluetooth_summary.average_score;
        score_count++;
    }
    
    if (score_count > 0) {
        overall_score /= score_count;
    }
    
    printf("\n=== OVERALL RESULTS ===\n");
    printf("Total Tests: %d\n", total_tests);
    printf("Passed: %d\n", total_passed);
    printf("Failed: %d\n", total_failed);
    printf("Success Rate: %.1f%%\n", (double)total_passed / total_tests * 100.0);
    printf("Overall Score: %.1f/100\n", overall_score);
    
    // Final verdict
    printf("\n=== FINAL VERDICT ===\n");
    if (total_failed == 0) {
        printf("🎉 ALL TESTS PASSED! Device is working perfectly.\n");
    } else if (total_passed > total_failed) {
        printf("✅ MOST TESTS PASSED! Device is working well with minor issues.\n");
    } else {
        printf("⚠️  MANY TESTS FAILED! Device has significant issues.\n");
    }
    
    printf("\n=== RECOMMENDATIONS ===\n");
    if (camera_summary.failed_tests > 0) {
        printf("- Camera issues detected. Check camera hardware and drivers.\n");
    }
    if (network_summary.failed_tests > 0) {
        printf("- Network issues detected. Check network configuration and connectivity.\n");
    }
    if (serial_summary.failed_tests > 0) {
        printf("- Serial issues detected. Check serial device connections and permissions.\n");
    }
    if (wiegand_summary.failed_tests > 0) {
        printf("- Wiegand issues detected. Check Wiegand device connections and GPIO configuration.\n");
    }
    if (lcd_summary.failed_tests > 0) {
        printf("- LCD issues detected. Check LCD connections and display drivers.\n");
    }
    if (cpu_summary.failed_tests > 0) {
        printf("- CPU issues detected. Check CPU configuration and thermal management.\n");
    }
    if (emmc_summary.failed_tests > 0) {
        printf("- eMMC issues detected. Check eMMC connections and storage health.\n");
    }
    if (speaker_summary.failed_tests > 0) {
        printf("- Speaker issues detected. Check audio hardware and drivers.\n");
    }
    if (led_summary.failed_tests > 0) {
        printf("- LED issues detected. Check GPIO connections and permissions.\n");
    }
    if (bluetooth_summary.failed_tests > 0) {
        printf("- Bluetooth issues detected. Check Bluetooth hardware and drivers.\n");
    }
    if (camera_summary.average_score < 50.0) {
        printf("- Camera performance is poor. Consider upgrading camera hardware.\n");
    }
    if (network_summary.average_score < 50.0) {
        printf("- Network performance is poor. Check network infrastructure.\n");
    }
    if (serial_summary.average_score < 50.0) {
        printf("- Serial performance is poor. Check serial device configuration.\n");
    }
    if (wiegand_summary.average_score < 50.0) {
        printf("- Wiegand performance is poor. Check Wiegand device configuration.\n");
    }
    if (lcd_summary.average_score < 50.0) {
        printf("- LCD performance is poor. Check LCD configuration and drivers.\n");
    }
    if (cpu_summary.average_score < 50.0) {
        printf("- CPU performance is poor. Check CPU configuration and cooling.\n");
    }
    if (emmc_summary.average_score < 50.0) {
        printf("- eMMC performance is poor. Check eMMC configuration and health.\n");
    }
    if (speaker_summary.average_score < 50.0) {
        printf("- Speaker performance is poor. Check audio configuration and drivers.\n");
    }
    if (led_summary.average_score < 50.0) {
        printf("- LED performance is poor. Check GPIO configuration and connections.\n");
    }
    if (bluetooth_summary.average_score < 50.0) {
        printf("- Bluetooth performance is poor. Check Bluetooth configuration and drivers.\n");
    }
    if (overall_score >= 80.0) {
        printf("- Overall device performance is excellent!\n");
    } else if (overall_score >= 60.0) {
        printf("- Overall device performance is good.\n");
    } else {
        printf("- Overall device performance needs improvement.\n");
    }
}

void print_usage(const char* program_name) {
    printf("Usage: %s [options]\n", program_name);
    printf("\nOptions:\n");
    printf("-h                    Show this help message\n");
    printf("-d <device>           Device type (camera, network, serial, wiegand, lcd, cpu, emmc, speaker, led, bluetooth, auto)\n");
    printf("-c <index>            Camera index (default: 0)\n");
    printf("-n <interface>        Network interface name (e.g., eth0, wlan0)\n");
    printf("-s <device>           Serial device path (e.g., /dev/ttyUSB0)\n");
    printf("-b <baud_rate>        Serial baud rate (default: 115200)\n");
    printf("-w <data0> <data1>    Wiegand data pins (e.g., 17 18)\n");
    printf("-t <test>             Run specific test:\n");
    printf("                   Camera tests:\n");
    printf("                     init      - Test camera initialization\n");
    printf("                     capture   - Test camera capture\n");
    printf("                     resolution - Test camera resolution\n");
    printf("                     fps       - Test camera FPS\n");
    printf("                     all_resolutions - Test all available resolutions\n");
    printf("                     capabilities - Test camera capabilities\n");
    printf("                     supported_capabilities - Test only supported capabilities\n");
    printf("                   Network tests:\n");
    printf("                     init      - Test network initialization\n");
    printf("                     connectivity - Test network connectivity\n");
    printf("                     speed     - Test network speed\n");
    printf("                     packet_loss - Test packet loss\n");
    printf("                   Serial tests:\n");
    printf("                     init      - Test serial initialization\n");
    printf("                     comm      - Test serial communication\n");
    printf("                     loopback  - Test serial loopback\n");
    printf("                     speed     - Test serial speed\n");
    printf("                     error     - Test serial error handling\n");
    printf("                     config    - Test serial configuration\n");
    printf("                   Wiegand tests:\n");
    printf("                     init      - Test Wiegand initialization\n");
    printf("                     reading   - Test Wiegand data reading\n");
    printf("                     protocols - Test Wiegand protocols\n");
    printf("                     parity    - Test Wiegand parity validation\n");
    printf("                     speed     - Test Wiegand transmission speed\n");
    printf("                     error     - Test Wiegand error handling\n");
    printf("                   LCD tests:\n");
    printf("                     connection - Test LCD connection\n");
    printf("                     init      - Test LCD initialization\n");
    printf("                     resolution - Test LCD resolution\n");
    printf("                     capabilities - Test LCD capabilities\n");
    printf("                   CPU tests:\n");
    printf("                     architecture - Test CPU architecture\n");
    printf("                     cores     - Test CPU cores\n");
    printf("                     frequency - Test CPU frequency\n");
    printf("                     capabilities - Test CPU capabilities\n");
    printf("                   eMMC tests:\n");
    printf("                     detection - Test eMMC detection\n");
    printf("                     capacity  - Test eMMC capacity\n");
    printf("                     capabilities - Test eMMC capabilities\n");
    printf("                   Speaker tests:\n");
    printf("                     detection - Test speaker detection\n");
    printf("                     capabilities - Test speaker capabilities\n");
    printf("                   LED tests:\n");
    printf("                     detection - Test LED detection\n");
    printf("                     capabilities - Test LED capabilities\n");
    printf("                   Bluetooth tests:\n");
    printf("                     detection - Test Bluetooth detection\n");
    printf("                     capabilities - Test Bluetooth capabilities\n");
    printf("                     all       - Run all tests for selected device\n");
    printf("                     auto      - Run automated test suite\n");
    printf("-i                    Start interactive mode\n");
    printf("\nExamples:\n");
    printf("  %s -t auto                    # Run automated test suite\n", program_name);
    printf("  %s -d camera -t all           # Run all camera tests\n", program_name);
    printf("  %s -d network -n eth0 -t all  # Run all network tests on eth0\n", program_name);
    printf("  %s -d serial -s /dev/ttyUSB0 -b 115200 -t all  # Run all serial tests\n", program_name);
    printf("  %s -d wiegand -w 17 18 -t all # Run all Wiegand tests\n", program_name);
    printf("  %s -d lcd -t all              # Run all LCD tests\n", program_name);
    printf("  %s -d cpu -t all              # Run all CPU tests\n", program_name);
    printf("  %s -d emmc -p /dev/mmcblk0 -t all # Run all eMMC tests\n", program_name);
    printf("  %s -d speaker -t all          # Run all speaker tests\n", program_name);
    printf("  %s -d led -l 17 18 -t all     # Run all LED tests\n", program_name);
    printf("  %s -d bluetooth -t all        # Run all Bluetooth tests\n", program_name);
    printf("  %s -t auto -s /dev/ttyUSB0 -b 9600 -w 17 18  # Run automated test suite with serial and Wiegand\n", program_name);
    printf("  %s -i                         # Start interactive camera mode\n", program_name);
}

int main(int argc, char* argv[]) {
    int camera_index = 0;
    char* device_type = NULL;
    char* network_interface = NULL;
    char* serial_device = NULL;
    int serial_baud_rate = 115200;
    int wiegand_data0_pin = -1;
    int wiegand_data1_pin = -1;
    char* test_type = NULL;
    char* device_path = NULL;
    int led1_pin = -1;
    int led2_pin = -1;
    bool interactive_mode = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            device_type = argv[++i];
        } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            camera_index = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            network_interface = argv[++i];
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            serial_device = argv[++i];
        } else if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) {
            serial_baud_rate = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-w") == 0 && i + 2 < argc) {
            wiegand_data0_pin = atoi(argv[++i]);
            wiegand_data1_pin = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            device_path = argv[++i];
        } else if (strcmp(argv[i], "-l") == 0 && i + 2 < argc) {
            led1_pin = atoi(argv[++i]);
            led2_pin = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            test_type = argv[++i];
        } else if (strcmp(argv[i], "-i") == 0) {
            interactive_mode = true;
        }
    }
    
    // Default to camera if no device type specified
    if (!device_type) {
        device_type = "camera";
    }
    
    // Handle automated test suite
    if (test_type && strcmp(test_type, "auto") == 0) {
        if (!network_interface) {
            network_interface = "lo"; // Default to loopback if no interface specified
        }
        run_automated_tests(camera_index, network_interface, serial_device, serial_baud_rate, wiegand_data0_pin, wiegand_data1_pin);
        return 0;
    }
    
    // Handle camera tests
    if (strcmp(device_type, "camera") == 0) {
        return handle_camera_commands(test_type, camera_index, interactive_mode);
    }
    // Handle serial tests
    else if (strcmp(device_type, "serial") == 0) {
        if (!serial_device) {
            printf("Error: Serial device (-s) is required for serial tests\n");
            print_usage(argv[0]);
            return 1;
        }
        return handle_serial_commands(test_type, serial_device, serial_baud_rate, interactive_mode);
    }
    // Handle Wiegand tests
    else if (strcmp(device_type, "wiegand") == 0) {
        if (wiegand_data0_pin < 0 || wiegand_data1_pin < 0) {
            printf("Error: Wiegand data pins (-w) are required for Wiegand tests\n");
            print_usage(argv[0]);
            return 1;
        }
        return handle_wiegand_commands(test_type, wiegand_data0_pin, wiegand_data1_pin, interactive_mode);
    }
    // Handle network tests
    else if (strcmp(device_type, "network") == 0) {
        if (!network_interface) {
            printf("Error: Network interface (-n) is required for network tests\n");
            print_usage(argv[0]);
            return 1;
        }
        return handle_network_commands(test_type, network_interface, interactive_mode);
    }
    // Handle LCD tests
    else if (strcmp(device_type, "lcd") == 0) {
        return handle_lcd_commands(test_type, LCD_INTERFACE_HDMI, NULL, interactive_mode);
    }
    // Handle CPU tests
    else if (strcmp(device_type, "cpu") == 0) {
        return handle_cpu_commands(test_type, interactive_mode);
    }
    // Handle eMMC tests
    else if (strcmp(device_type, "emmc") == 0) {
        if (!device_path) {
            device_path = "/dev/mmcblk0"; // Default eMMC device
        }
        return handle_emmc_commands(test_type, device_path, interactive_mode);
    }
    // Handle speaker tests
    else if (strcmp(device_type, "speaker") == 0) {
        return handle_speaker_commands(test_type, device_path, interactive_mode);
    }
    // Handle LED tests
    else if (strcmp(device_type, "led") == 0) {
        if (led1_pin < 0 || led2_pin < 0) {
            led1_pin = 17; // Default LED pins
            led2_pin = 18;
        }
        return handle_led_commands(test_type, led1_pin, led2_pin, interactive_mode);
    }
    // Handle Bluetooth tests
    else if (strcmp(device_type, "bluetooth") == 0) {
        return handle_bluetooth_commands(test_type, interactive_mode);
    }
    else {
        printf("Error: Unknown device type '%s'\n", device_type);
        print_usage(argv[0]);
        return 1;
    }
    
    return 0;
}
