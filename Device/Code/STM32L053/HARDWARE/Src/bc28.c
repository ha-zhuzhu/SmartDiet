#include "bc28.h"
#include "usart.h"

// #include "wdg.h"

char *strx, *extstrx;
BC28 BC28_Status;
uint8_t socketnum = 0; //当前的socket号码

/* 清空缓存 */
void Clear_LPUART1_Buffer_2_USART1(void)
{
    uint8_t i;
    USART1_SendStr(LPUART1_RX_BUF);
    for (i = 0; i < 200; i++)
        LPUART1_RX_BUF[i] = 0; //缓存
    RxCounter = 0;
    //IWDG_Feed(); //喂狗
}

uint8_t BC28_Init(void)
{
    if (LL_IOP_GRP1_IsEnabledClock(LL_IOP_GRP1_PERIPH_GPIOB) == 0)
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

    /* PB1 BC28_RSI
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTB, LL_SYSCFG_EXTI_LINE1);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_1, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_1, LL_GPIO_MODE_INPUT);

    LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
    EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_1;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&EXTI_InitStruct);
    */

    /* PB2 BC28_RST*/
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    LPUART1_SendStr("AT\r\n");
    LL_mDelay(300);
    strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"OK"); //返回OK
    Clear_Buffer();
    while (strx == NULL)
    {
        Clear_Buffer();
        LPUART1_SendStr("AT\r\n");
        LL_mDelay(300);
        strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"OK"); //返回OK
    }
    LPUART1_SendStr("AT+CFUN=1\r\n"); //获取卡号，类似是否存在卡的意思，比较重要。
    LL_mDelay(300);
    LPUART1_SendStr("AT+CIMI\r\n"); //获取卡号，类似是否存在卡的意思，比较重要。
    LL_mDelay(300);
    strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"460"); //返460，表明识别到卡了
    Clear_Buffer();
    while (strx == NULL)
    {
        Clear_Buffer();
        LPUART1_SendStr("AT+CIMI\r\n"); //获取卡号，类似是否存在卡的意思，比较重要。
        LL_mDelay(300);
        strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"460"); //返回OK,说明卡是存在的
    }
    Clear_Buffer();
    LPUART1_SendStr("AT+CGATT=1\r\n"); //激活网络，PDP
    LL_mDelay(300);
    strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"OK"); //返OK
    Clear_Buffer();
    LPUART1_SendStr("AT+CGATT?\r\n"); //查询激活状态
    LL_mDelay(300);
    strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"+CGATT:1"); //返1
    Clear_Buffer();
    while (strx == NULL)
    {
        Clear_Buffer();
        LPUART1_SendStr("AT+CGATT?\r\n"); //获取激活状态
        LL_mDelay(300);
        strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"+CGATT:1"); //返回1,表明注网成功
    }
    LPUART1_SendStr("AT+CSQ\r\n"); //查看获取CSQ值
    LL_mDelay(300);
    strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"+CSQ"); //返回CSQ
    BC28_Status.CSQ = (strx[5] - 0x30) * 10 + (strx[6] - 0x30);        //获取CSQ
    uint8_t CSQ_Rtycounter = 0;
    while ((BC28_Status.CSQ == 99) || ((strx[5] - 0x30) == 0) && CSQ_Rtycounter < 4) //说明扫网失败
    {
        Clear_Buffer();
        LPUART1_SendStr("AT+CSQ\r\n");
        LL_mDelay(300);
        strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"+CSQ"); //返回CSQ
        BC28_Status.CSQ = (strx[5] - 0x30) * 10 + (strx[6] - 0x30);        //获取CSQ
        CSQ_Rtycounter++;
    }
    if (CSQ_Rtycounter < 4)
        BC28_Status.netstatus = 1;
    else
    {
        BC28_Status.netstatus = 0;
        USART1_SendStr("信号搜索失败，请查看原因!\r\n");
        /* 暂时用AT指令复位 */
        //LPUART1_SendStr("AT+NRB\r\n");
        LL_GPIO_SetOutputPin(BC28_RST_GPIO, BC28_RST_PIN);
        LL_mDelay(200);
        LL_GPIO_ResetOutputPin(BC28_RST_GPIO, BC28_RST_PIN);
        LL_mDelay(200); //没有信号就复位
    }
    Clear_Buffer();
    return BC28_Status.netstatus;
}

void BC28_CreateUDPSokcet(void) //创建sokcet
{
    /*
    uint8_t i;
    for (i = 0; i < 5; i++)
    {
        sprintf(LPUART1_TX_BUF, "AT+NSOCL=%c\r\n", i + 0x30);
        LPUART1_SendStr(LPUART1_TX_BUF); //关闭上一次socekt连接
        LL_mDelay(300);
    }
    */
    Clear_Buffer();
    LPUART1_SendStr("AT+NSOCR=DGRAM,17,3005,1\r\n"); //开启SOCKET连接
    LL_mDelay(100);
    BC28_Status.Socketnum = LPUART1_RX_BUF[2]; //获取当前的socket号码
    Clear_Buffer();
}

//数据发送函数
void BC28_UDPSend(uint8_t *len, uint8_t *data)
{
    sprintf(LPUART1_TX_BUF, "AT+NSOST=%c,114.115.148.172,9999,%s,%s\r\n", socketnum, len, data);
    LPUART1_SendStr(LPUART1_TX_BUF); //发送0socketIP和端口后面跟对应数据长度以及数据,
    LL_mDelay(300);
    strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"OK"); //返回OK
    while (strx == NULL)
    {
        strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"OK"); //返回OK
    }
    Clear_Buffer();
}

void BC28_RECData(void)
{

    strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"+QSONMI"); //返回+QSONMI，表明接收到UDP服务器发回的数据
    if (strx)
    {

        Clear_Buffer();
    }
}

//创建对象连接到平台
void BC28_CreateInstance(void)
{
    uint8_t i = 0;
    LPUART1_SendStr("AT+MIPLCREATE\r\n");
    LL_mDelay(300);
    strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"+MIPLCREATE:0"); //对象创建成功
    while (strx == NULL)
    {
        strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"+MIPLCREATE:0");
    }
    Clear_Buffer();

    /* 
     * AT+MIPLADDOBJ
     * 作用：用于在模组端添加一个待订阅的object及其所需的instance
     * 命令格式：AT+MIPLADDOBJ=<ref>,<objid>,<inscount>,<bitmap>,<atts>,<acts>
     * a. ref：设备实例ID
     * b. objid：Object ID
     * c. inscount：实例个数
     * d. bitmap：实例位图，字符串格式，每一个字符表示为一个实例，1表示可用，0表示不可用。
     * e. atts：属性个数，默认设置为0即可。
     * f. acts：操作个数，默认设置为0即可。
     */
    // LPUART1_SendStr("AT+MIPLADDOBJ=0,3311,2,\"11\",4,2\r\n");//创建3311的2个对象
    LPUART1_SendStr("AT+MIPLADDOBJ=0,3322,1,\"1\",9,0\r\n"); //3322 Load 1 object
    LL_mDelay(300);
    strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"OK"); //对象创建成功
    while (strx == NULL)
    {
        strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"OK");
    }
    Clear_Buffer();
    /*
     * AT+MIPLOPEN
     * 作用：用于在模组端向OneNET 平台发起注册登录请求。
     * 命令格式：AT+MIPLOPEN=<ref>,<lifetime>[,<timeout>]
     * lifetime：本次注册平台的生命周期，单位是s
     * timeout：注册的超时时长，可选参数，默认为30，单位为s
     */
    LPUART1_SendStr("AT+MIPLOPEN=0,600,60\r\n"); //连接平台资源
    LL_mDelay(300);

    /* 获取observe号码 */
    strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"+MIPLOBSERVE:"); //获取observe号码
    while (strx == NULL)
    {
        strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"+MIPLOBSERVE:"); //获取observe号码
    }
    LL_mDelay(100);
    while (strx[16 + i] != ',') //没有遇到逗号就是observe ID
    {
        BC28_Status.Observe_ID[i] = strx[16 + i];
        i++;
    }
    Clear_Buffer(); //不能够放在while之前，因为strx本质是LPUART1_RX_BUF的指针，清零了就没了
    i = 0;

    /* 获取资源号码 */
    strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"+MIPLDISCOVER:"); //获取资源号码
    while (strx == NULL)
    {
        strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"+MIPLDISCOVER:"); //获取资源号码
    }
    LL_mDelay(100);
    while (strx[17 + i] != ',') //没有遇到逗号就是资源ID
    {
        BC28_Status.Resource_ID[i] = strx[17 + i];
        i++;
    }
    Clear_Buffer();
    LL_mDelay(1000);

    /* 注册 5701"Sensor Units" 5821"Current Calibration"属性到平台端 */
    //9 字符串位长
    sprintf(LPUART1_TX_BUF, "AT+MIPLDISCOVERRSP=0,%s,1,9,\"5701;5821\"\r\n", BC28_Status.Resource_ID);
    LPUART1_SendStr(LPUART1_TX_BUF);
    LL_mDelay(300);
    strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"OK");
    while (strx == NULL)
    {
        strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"OK");
    }
    Clear_Buffer();
}

void ONENET_Readdata(void) //等待服务器的读请求
{
    uint8_t i = 0, count = 0;
    strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"+MIPLREAD:"); //有读请求产生,这里根据不同的请求发送不同的指令需求
    if (strx)                                                                //读取到指令请求之后，获取内容数据
    {
        LL_mDelay(100);
        while (1)
        {
            if (strx[11 + i] == ',')
                count++;
            BC28_Status.ReadSource[i] = strx[11 + i];
            if (count >= 2) //遇到2个逗号了，需要跳出
            {
                BC28_Status.ReadSource[i + 1] = '1';
                break; //跳出
            }
            i++;
        }
        while (strx[11 + i] != 0x0D) //没有碰到结束符
        {

            BC28_Status.ReadSource[i + 2] = strx[11 + i]; //指针移动往后多移动三个位置，继续接收下面的数据
            USART1_SendStr(LPUART1_RX_BUF);
            i++;
        }
        strx = strchr(strx + 1, ',');
        Clear_Buffer();

        /* 
         * AT+MIPLREADRSP
         * 作用：MCU完成相应的Read操作后，向平台回复Read操作结果
         * AT+MIPLREADRSP=<ref>,<msgid>,<result>[,<objid>,<insid>,<resid>,<type>,<len>,<value>,<index>,<flag>]
         * result
         */
        // LPUART1_SendStr("AT+MIPLREADRSP=%s,4,4,2.35,0,0\r\n", BC28_Status.ReadSource); //回复请求，将数据传输上去
        sprintf(LPUART1_TX_BUF, "AT+MIPLREADRSP=%s,4,4,2.35,0,0\r\n", BC28_Status.ReadSource);
        LPUART1_SendStr(LPUART1_TX_BUF); //返回 2.35 4-float 4-数据位宽
        LL_mDelay(300);
        strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"OK"); //
        while (strx == NULL)
        {
            strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"OK"); //
        }
        Clear_Buffer();
    }
}

void BC28_NotifyResource(uint16_t ResourceValue, Resource_Typedef ResTyp)
{
    /*
     * AT+MIPLNOTIFY
     * 作用：用于在模组端向OneNET 平台上报指定资源的数据
     * AT+MIPLNOTIFY=<ref>,<msgid>,<objid>,<insid>,<resid>,<type>,<len>,<value>,<index>,<flag>[,<ackid>]
     * a. ref：设备实例ID
     * b. msgid：该resource所属的instance observe操作时下发的msgid
     * c. objid：Object ID
     * d. insid：Instance ID
     * e. resid：Resource ID
     * f. type：上报资源的数据类型（1-string，2-opaque，3-integer，4-float，5-bool，6-hex_str）
     * g. len：value值的长度
     * h. index：指令序号。可以发N条报文，从N-1到0降序编号，0表示本次Notify指令结束
     * i. value：上报数据。
     * j. flag：消息标识，指示第一条或中间或最后一条报文。
     */
    // 因为未知的原因，向ONENET上传float时len只能4位，且不能用引号。value的实际长度可以<=4位
    if (ResTyp == ResTyp_Weight) //weight 5位string 0补空
        sprintf(LPUART1_TX_BUF, "AT+MIPLNOTIFY=0,%s,3322,0,5701,1,5,\"%05d\",0,0\r\n", BC28_Status.Observe_ID, ResourceValue);
    if (ResTyp == ResTyp_VDDA) //vdda 4位string 0补空
        sprintf(LPUART1_TX_BUF, "AT+MIPLNOTIFY=0,%s,3322,0,5821,1,4,\"%04d\",0,0\r\n", BC28_Status.Observe_ID, ResourceValue);
    LPUART1_SendStr(LPUART1_TX_BUF);
    USART1_SendStr("notifysent\r\n");
    LL_mDelay(200); //配合 LCD_BlinkFre=2Hz 闪两下
    strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"OK");
    extstrx = strstr((const char *)LPUART1_RX_BUF, (const char *)"+NPSMR:0");
    while (strx == NULL && extstrx == NULL)
    {
        strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"OK");
        extstrx = strstr((const char *)LPUART1_RX_BUF, (const char *)"+NPSMR:0");
        Clear_Buffer();
    }
    Clear_Buffer();

    //如果每次notify时都处于PSM状态，重新连接需要2s，有点降低体验
}

void BC28_EnablePSM(void)
{
    /* 1-开启; TAU 01000001-10小时; active-timer 00000001 2s */
    LPUART1_SendStr("AT+CPSMS=1,,,01000111,00000001\r\n");
    LL_mDelay(50);
    /* 省电状态变化上报 */
    LPUART1_SendStr("AT+NPSMR=1\r\n");
    LL_mDelay(50);
    /* 使能URC上报功能 */
    //LPUART1_SendStr("AT+CSCON=1\r\n");
    //LL_mDelay(50);
    /* 创建UDP */
    BC28_CreateUDPSokcet();
    //LL_mDelay(200);
}

void BC28_DisablePSM(void)
{
    LPUART1_SendStr("AT+CPSMS=0\r\n");
    LL_mDelay(100);
    LPUART1_SendStr("AT+NPSMR=0\r\n");
}

void BC28_Sleep(void)
{
    /* 0x200发送立即释放 400收到后释放 socket号 */
    sprintf(LPUART1_TX_BUF, "AT+NSOSTF=%c,47.92.146.210,8888,0x200,2,AB30\r\n", BC28_Status.Socketnum);
    LPUART1_SendStr(LPUART1_TX_BUF); //发送0socketIP和端口后面跟对应数据长度以及数据,
    // 如果之前处于PSM模式，那么一共需要4s（重新连接2s+activetimer 2s）
    LL_mDelay(2000);
    // 如果要在中断处理函数中运行Sleep，记得把LP串口接收中断优先级调高
    strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"+NPSMR:1");
    while (strx == NULL)
    {
        strx = strstr((const char *)LPUART1_RX_BUF, (const char *)"+NPSMR:1");
    }
    Clear_Buffer();
}