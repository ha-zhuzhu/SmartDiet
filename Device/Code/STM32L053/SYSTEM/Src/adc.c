#include "adc.h"

void ADC_Configure(void)
{
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
    LL_ADC_InitTypeDef ADC_InitStruct = {0};

    /* Peripheral clock enable */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);

    /** Configure Regular Channel */
    LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_VREFINT);
    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_VREFINT);

    /** Common config */
    ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
    ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_1RANK;
    ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
    ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
    ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;
    LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
    LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_160CYCLES_5);
    LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);
    LL_ADC_REG_SetSequencerScanDirection(ADC1, LL_ADC_REG_SEQ_SCAN_DIR_FORWARD);
    LL_ADC_SetCommonFrequencyMode(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_FREQ_MODE_HIGH);
    LL_ADC_DisableIT_EOC(ADC1);
    LL_ADC_DisableIT_EOS(ADC1);
    ADC_InitStruct.Clock = LL_ADC_CLOCK_SYNC_PCLK_DIV4;
    ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
    ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
    LL_ADC_Init(ADC1, &ADC_InitStruct);

    /* Disable ADC deep power down (enabled by default after reset state) */
    //LL_ADC_DisableDeepPowerDown(ADC1);
    /* Enable ADC internal voltage regulator */
    //LL_ADC_EnableInternalRegulator(ADC1);
    /* Delay for ADC internal voltage regulator stabilization. */
    /* Compute number of CPU cycles to wait for, from delay in us. */
    /* Note: Variable divided by 2 to compensate partially */
    /* CPU processing cycles (depends on compilation optimization). */
    /* Note: If system core clock frequency is below 200kHz, wait time */
    /* is only a few CPU processing cycles. */
}

void ADC_Activate(void)
{
    __IO uint32_t wait_loop_index = 0;
#if (USE_TIMEOUT == 1)
    uint32_t Timeout = 0; /* Variable used for timeout management */
#endif                    /* USE_TIMEOUT */

    /*## Operation on ADC hierarchical scope: ADC instance #####################*/

    /* Note: Hardware constraint (refer to description of the functions         */
    /*       below):                                                            */
    /*       On this STM32 serie, setting of these features is conditioned to   */
    /*       ADC state:                                                         */
    /*       ADC must be disabled.                                              */
    /* Note: In this example, all these checks are not necessary but are        */
    /*       implemented anyway to show the best practice usages                */
    /*       corresponding to reference manual procedure.                       */
    /*       Software can be optimized by removing some of these checks, if     */
    /*       they are not relevant considering previous settings and actions    */
    /*       in user application.                                               */
    if (LL_ADC_IsEnabled(ADC1) == 0)
    {
        /* Disable ADC deep power down (enabled by default after reset state) */
        //LL_ADC_DisableDeepPowerDown(ADC1);

        /* Enable ADC internal voltage regulator */
        LL_ADC_EnableInternalRegulator(ADC1);

        /* Delay for ADC internal voltage regulator stabilization.                */
        /* Compute number of CPU cycles to wait for, from delay in us.            */
        /* Note: Variable divided by 2 to compensate partially                    */
        /*       CPU processing cycles (depends on compilation optimization).     */
        /* Note: If system core clock frequency is below 200kHz, wait time        */
        /*       is only a few CPU processing cycles.                             */
        wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
        while (wait_loop_index != 0)
        {
            wait_loop_index--;
        }

        /* Run ADC self calibration */
        LL_ADC_StartCalibration(ADC1);

/* Poll for ADC effectively calibrated */
#if (USE_TIMEOUT == 1)
        Timeout = ADC_CALIBRATION_TIMEOUT_MS;
#endif /* USE_TIMEOUT */

        while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
        {
#if (USE_TIMEOUT == 1)
            /* Check Systick counter flag to decrement the time-out value */
            if (LL_SYSTICK_IsActiveCounterFlag())
            {
                if (Timeout-- == 0)
                {
                    /* Time-out occurred. Set LED to blinking mode */
                    LED_Blinking(LED_BLINK_ERROR);
                }
            }
#endif /* USE_TIMEOUT */
        }

        /* Delay between ADC end of calibration and ADC enable.                   */
        /* Note: Variable divided by 2 to compensate partially                    */
        /*       CPU processing cycles (depends on compilation optimization).     */
        wait_loop_index = (ADC_DELAY_CALIB_ENABLE_CPU_CYCLES >> 1);
        while (wait_loop_index != 0)
        {
            wait_loop_index--;
        }

        /* Enable ADC */
        LL_ADC_Enable(ADC1);

/* Poll for ADC ready to convert */
#if (USE_TIMEOUT == 1)
        Timeout = ADC_ENABLE_TIMEOUT_MS;
#endif /* USE_TIMEOUT */

        while (LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0)
        {
#if (USE_TIMEOUT == 1)
            /* Check Systick counter flag to decrement the time-out value */
            if (LL_SYSTICK_IsActiveCounterFlag())
            {
                if (Timeout-- == 0)
                {
                    /* Time-out occurred. Set LED to blinking mode */
                    LED_Blinking(LED_BLINK_ERROR);
                }
            }
#endif /* USE_TIMEOUT */
        }

        /* Note: ADC flag ADRDY is not cleared here to be able to check ADC       */
        /*       status afterwards.                                               */
        /*       This flag should be cleared at ADC Deactivation, before a new    */
        /*       ADC activation, using function "LL_ADC_ClearFlag_ADRDY()".       */
    }

    /*## Operation on ADC hierarchical scope: ADC group regular ################*/
    /* Note: No operation on ADC group regular performed here.                  */
    /*       ADC group regular conversions to be performed after this function  */
    /*       using function:                                                    */
    /*       "LL_ADC_REG_StartConversion();"                                    */

    /*## Operation on ADC hierarchical scope: ADC group injected ###############*/
    /* Note: No operation on ADC group injected performed here.                 */
    /*       ADC group injected conversions to be performed after this function */
    /*       using function:                                                    */
    /*       "LL_ADC_INJ_StartConversion();"                                    */
}

/**
  * @brief  Perform ADC de-activation procedure to save power consumption
  * @note   Operations:
  *         - ADC instance
  *           - Disable ADC
  *           - Save ADC calibration factors
  *           - Disable internal voltage regulator
  *           - Enable deep power down
  *         - ADC group regular
  *           none: ADC conversion start-stop to be performed
  *                 before this function
  *         - ADC group injected
  *           none: ADC conversion start-stop to be performed
  *                 before this function
  * @param  None
  * @retval None
  */
void ADC_DeActivate(void)
{
#if (USE_TIMEOUT == 1)
    uint32_t Timeout = 0U; /* Variable used for timeout management */
#endif                     /* USE_TIMEOUT */

    /*## Operation on ADC hierarchical scope: ADC instance #####################*/

    /* Note: Hardware constraint (refer to description of the functions         */
    /*       below):                                                            */
    /*       On this STM32 serie, setting of these features is conditioned to   */
    /*       ADC state:                                                         */
    /*       ADC must be enabled.                                               */
    /* Note: In this example, all these checks are not necessary but are        */
    /*       implemented anyway to show the best practice usages                */
    /*       corresponding to reference manual procedure.                       */
    /*       Software can be optimized by removing some of these checks, if     */
    /*       they are not relevant considering previous settings and actions    */
    /*       in user application.                                               */
    if (LL_ADC_IsEnabled(ADC1) != 0)
    {
        /* Disable ADC */
        LL_ADC_Disable(ADC1);

/* Poll for ADC effectively disabled */
#if (USE_TIMEOUT == 1)
        Timeout = ADC_DISABLE_TIMEOUT_MS;
#endif /* USE_TIMEOUT */

        while (LL_ADC_IsDisableOngoing(ADC1) != 0)
        {
#if (USE_TIMEOUT == 1)
            /* Check Systick counter flag to decrement the time-out value */
            if (LL_SYSTICK_IsActiveCounterFlag())
            {
                if (Timeout-- == 0)
                {
                    /* Time-out occurred. Set LED to blinking mode */
                    LED_Blinking(LED_BLINK_ERROR);
                }
            }
#endif /* USE_TIMEOUT */
        }

        /* Disable ADC internal voltage regulator */
        LL_ADC_DisableInternalRegulator(ADC1);

        /* Enable ADC deep power down (enabled by default after reset state) */
        //LL_ADC_EnableDeepPowerDown(ADC1);

        /* Clear flags */
        LL_ADC_ClearFlag_ADRDY(ADC1);
        LL_ADC_ClearFlag_EOC(ADC1);
        LL_ADC_ClearFlag_EOS(ADC1);
    }

    /*## Operation on ADC hierarchical scope: ADC group regular ################*/
    /* Note: No operation on ADC group regular performed here.                  */
    /*       ADC group regular conversions to be performed after this function  */
    /*       using function:                                                    */
    /*       "LL_ADC_REG_StartConversion();"                                    */

    /*## Operation on ADC hierarchical scope: ADC group injected ###############*/
    /* Note: No operation on ADC group injected performed here.                 */
    /*       ADC group injected conversions to be performed after this function */
    /*       using function:                                                    */
    /*       "LL_ADC_INJ_StartConversion();"                                    */
}

/**
  * @brief  Perform ADC group regular conversion start, poll for conversion
  *         completion.
  *         (ADC instance: ADC1).
  * @note   This function does not perform ADC group regular conversion stop:
  *         intended to be used with ADC in single mode, trigger SW start
  *         (only 1 ADC conversion done at each trigger, no conversion stop
  *         needed).
  *         In case of continuous mode or conversion trigger set to 
  *         external trigger, ADC group regular conversion stop must be added.
  * @param  None
  * @retval None
  */
void ConversionStartPoll_ADC_GrpRegular(void)
{
#if (USE_TIMEOUT == 1)
    uint32_t Timeout = 0; /* Variable used for timeout management */
#endif                    /* USE_TIMEOUT */

    /* Start ADC group regular conversion */
    /* Note: Hardware constraint (refer to description of the function          */
    /*       below):                                                            */
    /*       On this STM32 serie, setting of this feature is conditioned to     */
    /*       ADC state:                                                         */
    /*       ADC must be enabled without conversion on going on group regular,  */
    /*       without ADC disable command on going.                              */
    /* Note: In this example, all these checks are not necessary but are        */
    /*       implemented anyway to show the best practice usages                */
    /*       corresponding to reference manual procedure.                       */
    /*       Software can be optimized by removing some of these checks, if     */
    /*       they are not relevant considering previous settings and actions    */
    /*       in user application.                                               */
    if ((LL_ADC_IsEnabled(ADC1) == 1) &&
        (LL_ADC_IsDisableOngoing(ADC1) == 0) &&
        (LL_ADC_REG_IsConversionOngoing(ADC1) == 0))
    {
        LL_ADC_REG_StartConversion(ADC1);
    }
    else
    {
        /* Error: ADC conversion start could not be performed */
        Error_Handler();
    }

#if (USE_TIMEOUT == 1)
    Timeout = ADC_UNITARY_CONVERSION_TIMEOUT_MS;
#endif /* USE_TIMEOUT */

    while (LL_ADC_IsActiveFlag_EOC(ADC1) == 0)
    {
#if (USE_TIMEOUT == 1)
        /* Check Systick counter flag to decrement the time-out value */
        if (LL_SYSTICK_IsActiveCounterFlag())
        {
            if (Timeout-- == 0)
            {
                /* Time-out occurred. Set LED to blinking mode */
                LED_Blinking(LED_BLINK_SLOW);
            }
        }
#endif /* USE_TIMEOUT */
    }

    /* Clear flag ADC group regular end of unitary conversion */
    /* Note: This action is not needed here, because flag ADC group regular   */
    /*       end of unitary conversion is cleared automatically when          */
    /*       software reads conversion data from ADC data register.           */
    /*       Nevertheless, this action is done anyway to show how to clear    */
    /*       this flag, needed if conversion data is not always read          */
    /*       or if group injected end of unitary conversion is used (for      */
    /*       devices with group injected available).                          */
    //LL_ADC_ClearFlag_EOC(ADC1);
}

uint16_t VDDA_Get(void)
{

    uint16_t vref, adc_vref_sum, vdda;
    vref = *(__IO uint16_t *)VREFINT_ADDR;
    adc_vref_sum = 0;
    ADC_Activate();
    for (uint8_t i = 0; i < VDDA_ADC_Times; ++i)
    {
        ConversionStartPoll_ADC_GrpRegular();
        adc_vref_sum += LL_ADC_REG_ReadConversionData12(ADC1);
    }
    vdda = vref * 1e3 * VDDA_ADC_Times / adc_vref_sum * 3;
    ADC_DeActivate();
    return vdda;
}