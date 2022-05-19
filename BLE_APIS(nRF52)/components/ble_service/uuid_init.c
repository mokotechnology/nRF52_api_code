#include "uuid_init.h"
#include "sdk_common.h"
#include "ble_srv_common.h"
#include "Platform_bleInit.h"
#include "ble.h"
#include <stdint.h>
#include <string.h>



uint8_t uuid_init(ble_nus_t * p_nus, const ble_nus_init_t * p_nus_init)
{
	uint32_t err_code;
	
	// Add the more Characteristic.
	err_code = add_uuid(p_nus, p_nus_init, char_UUID, &p_nus->uuid_hand, 1, 1,    \
	           p_nus->user_data.UUID, sizeof(p_nus->user_data.UUID));
	VERIFY_SUCCESS(err_code);
	
	err_code = add_uuid(p_nus, p_nus_init, char_Maior, &p_nus->major_hand, 1, 1,   \
	           p_nus->user_data.Major, sizeof(p_nus->user_data.Major));
	VERIFY_SUCCESS(err_code);
	

	return NRF_SUCCESS;
}

