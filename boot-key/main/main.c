#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void *arg)
{
    uint32_t io_num;
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            printf("GPIO[%"PRIu32"] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}

void app_main(void)
{
    gpio_config_t io_conf = {};

    // 中断关闭
    // io_conf.intr_type = GPIO_INTR_DISABLE;
    // 下降沿触发
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    // 输出模式
    // io_conf.mode = GPIO_MODE_OUTPUT;
    // 输入模式
    io_conf.mode = GPIO_MODE_INPUT;
    // 配置GPIO引脚
    // io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    // 配置GPIO0引脚
    io_conf.pin_bit_mask = 1 << GPIO_NUM_0;
    // 禁用下拉电阻
    io_conf.pull_down_en = 0;
    // 启用下拉电阻
    // io_conf.pull_down_en = 1;
    // 禁用上拉电阻
    // io_conf.pull_up_en = 0;
    // 启用上拉电阻
    io_conf.pull_up_en = 1;

    gpio_config(&io_conf);

    // 创建GPIO事件队列
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    // 创建GPIO任务
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);
    // gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    // 启用GPIO中断服务
    gpio_install_isr_service(0);
    // 给GPIO_NUM_0添加中断处理程序
    gpio_isr_handler_add(GPIO_NUM_0, gpio_isr_handler, (void *)GPIO_NUM_0);
}