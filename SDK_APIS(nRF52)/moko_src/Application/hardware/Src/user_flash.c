#include "user_flash.h"
#include "in_flash_manage.h"
#include "nrf_delay.h"
#include "bsp.h"

STU_HIS StuHis;
/**************************************************
*function: user_data_init
* 
*************************************************/
static void user_data_init(void)
{
	memset(&StuHis,0,sizeof(STU_HIS));
	memset(StuHis.mac_addr,0xff,sizeof(StuHis.mac_addr));
	
	StuHis.Beacon_rssi  =-65;
	StuHis.tx_Power     = 0;
	StuHis.adv_interval =500;

	memcpy(StuHis.device_name,"Azuga DS",strlen("Azuga DS"));
	StuHis.device_name_len+=strlen("Azuga DS");
	
	memcpy(StuHis.manufactur_name_str,"MOKO TECHNOLOGY LTD.",strlen("MOKO TECHNOLOGY LTD."));

	memcpy(StuHis.product_type_str,"H2-CNCA-Azuga",strlen("H2-CNCA-Azuga"));
	memcpy(StuHis.software_verison_str,"H2-CNCA-Azuga",strlen("H2-CNCA-Azuga"));
	
	StuHis.produce_time[0]=2021>>8;
	StuHis.produce_time[1]=(uint8_t)2021; 
	StuHis.produce_time[2]=10;
	StuHis.produce_time[3]=10;
        
    StuHis.tof_interval = 1;
    StuHis.tof_mode = 1;
    StuHis.tof_times = 5;
    StuHis.tof_time = 32;
}

/******************************************************************
* function : read_user_flash
*@format:[head]
*        aa aa  len  len data data ......xor
*        0   1  2    3   4   5          len+4
*len_real=((sizeof(STU_HIS)+2) % 4 == 0)?(sizeof(STU_HIS)+2):(((sizeof(STU_HIS)+2)/ 4) * 4 + 4);
* 用户flash  data init
*************************************************************/
#define HIS_BUF_LEN ((sizeof(STU_HIS)+5+3)/4)
uint32_t read_flash_buf[HIS_BUF_LEN]={0,0,0,0,0,0,0};
void read_user_flash(void)
{	
    uint8_t *pdata=NULL;
    uint8_t *pdata_s=NULL;
    uint8_t *pdata_d=NULL;
    uint8_t data_xor,calc_xor;
    uint16_t len,len_real,i;
	
	// 1：read flash  data 
    InflashRead((uint8_t*)read_flash_buf,HIS_ADDR,HIS_BUF_LEN*4);

    pdata=(uint8_t*)read_flash_buf;

    if((pdata[0]==HIS_HEAD)&&(pdata[1]==HIS_HEAD))
    {
        len_real=sizeof(STU_HIS);		
	
        BLE_RTT("len_real=%d,buf_sizeof=%d\r\n",len_real,sizeof(read_flash_buf));

		// 2：get flash data len
        len=pdata[2];
        len=(len<<8)|pdata[3];

        if(len_real>len)
        {
			BLE_RTT("read falsh: len not true\r\n");
			goto RESET_HIS;
        }
		// 3:check sum
        data_xor=*(pdata+len+4);
        pdata_s=pdata+4;
        calc_xor=0;
        for(i=0; i<len; i++)
        {
            calc_xor^=pdata_s[i];
        }
        if(calc_xor!=data_xor)
        {
            BLE_RTT("read falsh: calc_xor not true\r\n");
        }
        pdata_d=(uint8_t*)&StuHis;
        for(i=0; i<len; i++)
        {
            pdata_d[i]=pdata_s[i];
        }
    }
	else
	{
RESET_HIS:
	    user_data_init();	
		ask_save_factory();
	}
}

/**************************************************
*function: write_user_flash
*description: write user  data  to flash
*************************************************/
uint32_t flash_write_buf[HIS_BUF_LEN]={0,0,0,0,0,0};
void write_user_flash(uint8_t set_mode)
{
    uint8_t *pdata;
    uint8_t *pdata_src;

    uint16_t len_real,i,total_len;
    uint8_t dataxor;

	memset(flash_write_buf,0,sizeof(flash_write_buf));
	
    pdata=(uint8_t*)flash_write_buf; //  buff

    pdata[0]=HIS_HEAD;
    pdata[1]=HIS_HEAD;

    len_real=sizeof(STU_HIS);	
	// 1：data len
    pdata[2]=len_real>>8;
    pdata[3]=len_real;

	// 2：StuHis
    pdata_src=(uint8_t*)&StuHis;   //

	//3：check sum
    dataxor=0;
    for(i=0; i<len_real; i++)
    {
        pdata[4+i]=pdata_src[i];
        dataxor^=pdata_src[i];
    }
    pdata[4+len_real]=dataxor;
    total_len=len_real+5;
	
	BLE_RTT("write_His: len_real=%d,buf_sizeof=%d\r\n",len_real,sizeof(flash_write_buf));
    {
        Flash_Erase_Page(HIS_ADDR/PAGE_SIZE);
        Flash_Write_World((uint32_t *)HIS_ADDR,flash_write_buf,(total_len+3)/4); //HIS_BUF_LEN
    }
	
	// write factory datas
	if(set_mode==2)
	{
	   write_factory_flash(flash_write_buf);
	}
}

/**************************************************
*function: read_factory_flash
*description:read user  data  form factory flash
*************************************************/
void read_factory_flash(void)
{
    uint8_t *pdes,*pdata;
    uint16_t i,len_real;
    pdes=(uint8_t*)&StuHis;

    if(*((uint32_t *)RSV1_ADDR) == FACTORY_SET_HEAD)//have factory data  aa aa aa aa h h lh ll datas.........
    {
        len_real=sizeof(STU_HIS);
	
        pdata=(uint8_t*)(RSV1_ADDR+4+4);
        pdes=(uint8_t*)&StuHis;

        for(i=0; i<len_real; i++)
        {
            pdes[i]=pdata[i];
        }
    }
    else
    {
        for(i=0; i<(sizeof(STU_HIS)); i++)
        {
            pdes[i]=0;
        }			
        user_data_init();	
    }
	ask_save_factory();
}

/**************************************************
*function: write_factory_flash
*description:read user  data  to factory flash
*************************************************/

static uint8_t save_factory_flg =0;
void write_factory_flash(uint32_t *pdata)
{
    uint32_t data_head=FACTORY_SET_HEAD;

    if(save_factory_flg)
    {
        save_factory_flg=0;
		
        Flash_Erase_Page(RSV1_ADDR/PAGE_SIZE);
        Flash_Write_World((uint32_t *)RSV1_ADDR,&data_head,1);
        Flash_Write_World((uint32_t *)(RSV1_ADDR+4),pdata,(sizeof(STU_HIS)+3+4)/4);
    }
}


/********************************************
*  
*    保存flash数据的任务
************************************************/
static uint8_t save_his_addr_flg=0;
void save_user_flash(void)
{
    save_his_addr_flg=1;
}
void ask_reset_his(void)
{
    save_his_addr_flg=2;
}

void ask_save_factory(void)
{
	save_factory_flg =1;
	save_his_addr_flg =1;
}


/***************************************************
*
*task_save_flash
****************************************/
void task_save_flash(void)
{
	if(save_his_addr_flg==0)  return;
		
	if((save_his_addr_flg==1)&&(save_factory_flg==1))
	{	

		nrf_delay_ms(100);
		
		BLE_RTT("save_factory_flg \r\n");
		write_user_flash(2);
		save_his_addr_flg =0;
		save_factory_flg =0;
	}
    if(save_his_addr_flg==1)
	{
		BLE_RTT("save_his_addr_flg \r\n");
        write_user_flash(1);
		save_his_addr_flg =0;
	}
	else if(save_his_addr_flg==2)
	{
		nrf_delay_ms(100);
	    read_factory_flash();		
				
		BLE_RTT("Read_factory_datas=2\r\n ");
		nrf_delay_ms(500);
		NVIC_SystemReset();
		save_his_addr_flg  =0;	
	}
}






