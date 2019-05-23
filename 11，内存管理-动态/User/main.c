/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2017-xx-xx
  * @brief   
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 ȫϵ�� ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32h7xx.h"

 /* LiteOS ͷ�ļ� */
#include "los_sys.h"
#include "los_task.ph"
#include "los_memory.h"

/* �弶����ͷ�ļ� */
#include "bsp_led.h"
#include "core_delay.h"  
#include "./key/bsp_key.h" 

/********************************** ������ *************************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */

/* ���������� */
UINT32 LED_Task_Handle;
UINT32 Key_Task_Handle;

/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */


/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
 UINT32 *p_Num = NULL;			//ָ���д�ڴ��ַ��ָ��
 
	
/******************************* �궨�� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩ�궨�塣
 */
/* ��غ궨�� */
#define  	MALLOC_MEM_SIZE   	16	//�����ڴ�Ĵ�С���ֽڣ�


/* �������� */
static UINT32 AppTaskCreate(void);
static UINT32 Creat_LED_Task(void);
static UINT32 Creat_Key_Task(void);

static void LED_Task(void);
static void Key_Task(void);

extern void BSP_Init(void);
/***************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
			�ڶ���������APPӦ������
			������������LiteOS����ʼ��������ȣ�����ʧ�������������Ϣ
  **************************************************************/
int main(void)
{	
	UINT32 uwRet = LOS_OK;  //����һ�����񴴽��ķ���ֵ��Ĭ��Ϊ�����ɹ�
	
	/* ������س�ʼ�� */
  BSP_Init();
	
	printf("����һ��[Ұ��]-STM32ȫϵ�п�����-LiteOS��̬�ڴ����ʵ�飡\n");
	printf("ϵͳ��ʼ����ʱ���Ѿ������ڴ��ʼ�������Դ�ʱ�����ʼ��\n");
	printf("����KEY1�����ڴ棬����KEY2�ͷ��ڴ棡\n\n");
	/* LiteOS �ں˳�ʼ�� */
	uwRet = LOS_KernelInit();
	
  if (uwRet != LOS_OK)
  {
		printf("LiteOS ���ĳ�ʼ��ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return LOS_NOK;
  }

	/* ����APPӦ���������е�Ӧ�����񶼿��Է�������������� */
	uwRet = AppTaskCreate();
	if (uwRet != LOS_OK)
  {
		printf("AppTaskCreate��������ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return LOS_NOK;
  }

  /* ����LiteOS������� */
  LOS_Start();
	
	//��������²���ִ�е�����
	while(1);
}


/*********************************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� ���񴴽���Ϊ�˷���������е����񴴽����������Է��������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  ********************************************************************************/
static UINT32 AppTaskCreate(void)
{
	/* ����һ���������ͱ�������ʼ��ΪLOS_OK */
	UINT32 uwRet = LOS_OK;
	
	
	uwRet = Creat_LED_Task();
  if (uwRet != LOS_OK)
  {
		printf("LED_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return uwRet;
  }
	
	uwRet = Creat_Key_Task();
  if (uwRet != LOS_OK)
  {
		printf("Key_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return uwRet;
  }
	return LOS_OK;
}


/******************************************************************
  * @ ������  �� Creat_LED_Task
  * @ ����˵���� ����LED_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_LED_Task()
{
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;			
	
	//����һ�����ڴ�������Ĳ����ṹ��
	TSK_INIT_PARAM_S task_init_param;	

	task_init_param.usTaskPrio = 5;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "LED_Task";/* ������ */
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)LED_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;		/* ��ջ��С */

	uwRet = LOS_TaskCreate(&LED_Task_Handle, &task_init_param);/* �������� */
	return uwRet;
}
/*******************************************************************
  * @ ������  �� Creat_Key_Task
  * @ ����˵���� ����Key_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_Key_Task()
{
	// ����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 4;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "Key_Task";	/* ������*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Key_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;	/* ��ջ��С */
	
	uwRet = LOS_TaskCreate(&Key_Task_Handle, &task_init_param);/* �������� */

	return uwRet;
}

/******************************************************************
  * @ ������  �� LED_Task
  * @ ����˵���� LED_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void LED_Task(void)
{
	// ����һ���¼����ձ���
	UINT32 uwRet;			
  /* ������һ������ѭ�������ܷ��� */
	while(1)
	{
    LED2_TOGGLE;
    LOS_TaskDelay(1000);/* ��ʱ1000��tick */
	}
}
/******************************************************************
  * @ ������  �� Key_Task
  * @ ����˵���� Key_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void Key_Task(void)
{			
	// ����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;			

	/* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		/* KEY1 ������ */
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )       
		{
      if(NULL == p_Num)
      {
				printf("���������ڴ�....................\n");  
				p_Num = (UINT32*)LOS_MemAlloc(m_aucSysMem0,MALLOC_MEM_SIZE);	
					
				if (NULL == p_Num)
					printf("�����ڴ�ʧ��!\n");
				else
				{
					printf("�����ڴ�ɹ�!��ַΪ0x%X \n",(uint32_t)p_Num);
					
					//��Test_Ptr��д�뵱����:��ǰϵͳʱ��
					sprintf((char*)p_Num,"��ǰϵͳTickCount = %d",(UINT32)LOS_TickCountGet());
					printf("д��������� %s \n\n",(char*)p_Num);
				}
			}
			else
				printf("���Ȱ���KEY2�ͷ��ڴ�������\n");
		}

    /* KEY2 ������ */
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON )      
		{
      if(NULL != p_Num)
      {
				printf("�����ͷ��ڴ�....................\n");
				uwRet = LOS_MemFree(m_aucSysMem0,p_Num);
				if (LOS_OK == uwRet)
				{
					printf("�ڴ��ͷųɹ�!\n\n");//�ڴ��ͷųɹ���
					p_Num = NULL;
				}
				else
				{
					printf("�ڴ��ͷ�ʧ��!\n\n");//�ڴ��ͷ�ʧ�ܣ�
				}
			}
			else
        printf("���Ȱ���KEY1�����ڴ����ͷ�\n\n");
		}
		
		LOS_TaskDelay(20);     //ÿ20msɨ��һ��		
	}
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
