#include "ga_dev_xbee.h"

struct XBee xbee;

void ga_dev_xbee_open(void)
{
    soft_serial.begin(9600);
    xbee.serial = &soft_serial;
    xbee.api_id = ZB_TX_REQUEST;
    xbee.frame_id = DEFAULT_FRAME_ID;
    xbee.broadcast_range = ZB_BROADCAST_RADIUS_MAX_HOPS;
    xbee.option = ZB_TX_UNICAST;
    xbee.addr16 = ZB_BROADCAST_PACKET;
    xbee.msb = 0;
    xbee.lsb = 0;
}

int ga_dev_xbee_avail(void) {
    return Serial.available();
}

int ga_dev_xbee_read(void) {
    return Serial.read();
}

void ga_dev_xbee_write(uint8_t val) {
    xbee.serial -> write(val);
}

uint8_t ga_dev_xbee_frame_length(int data_len) {
    return ZB_TX_API_LENGTH + data_len;
}

uint8_t ga_dev_xbee_frame_data(uint8_t* data, int pos) {
    if (pos == 0) {
		return (xbee.msb >> 24) & 0xff;
	}
    else if (pos == 1) {
		return (xbee.msb >> 16) & 0xff;
	}
    else if (pos == 2) {
		return (xbee.msb >> 8) & 0xff;
	}
    else if (pos == 3) {
		return xbee.msb & 0xff;
	}
    else if (pos == 4) {
		return (xbee.lsb >> 24) & 0xff;
	}
    else if (pos == 5) {
		return  (xbee.lsb >> 16) & 0xff;
	}
    else if (pos == 6) {
		return (xbee.lsb >> 8) & 0xff;
	}
    else if (pos == 7) {
		return xbee.lsb & 0xff;
	}
    else if (pos == 8) {
		return (xbee.addr16 >> 8) & 0xff;
	}
    else if (pos == 9) {
		return xbee.addr16 & 0xff;
	}
    else if (pos == 10) {
		return xbee.broadcast_range;
	}
    else if (pos == 11) {
        return xbee.option;
	}
    else {
		return data[pos - ZB_TX_API_LENGTH];
	}
}

void ga_dev_xbee_send_byte(uint8_t b, bool escape) {
    if (escape && (b == START_BYTE || b == ESCAPE || b == XON || b == XOFF)) {
        ga_dev_xbee_write(ESCAPE);
        ga_dev_xbee_write(b ^ 0x20);
    }

    else {
        ga_dev_xbee_write(b);
    }
}

void ga_dev_xbee_send(uint8_t* data, int data_len) {
    ga_dev_xbee_send_byte(START_BYTE, false);

	// send length
	uint8_t msbLen = ((ga_dev_xbee_frame_length(data_len) + 2) >> 8) & 0xff;
	uint8_t lsbLen = (ga_dev_xbee_frame_length(data_len) + 2) & 0xff;

	ga_dev_xbee_send_byte(msbLen, true);
	ga_dev_xbee_send_byte(lsbLen, true);

	// api id
	ga_dev_xbee_send_byte(xbee.api_id, true);
	ga_dev_xbee_send_byte(xbee.frame_id, true);

	uint8_t checksum = 0;

	// compute checksum, start at api id
	checksum+= xbee.api_id;
	checksum+= xbee.frame_id;

	for (int i = 0; i < ga_dev_xbee_frame_length(data_len); i++) {
	    ga_dev_xbee_send_byte(ga_dev_xbee_frame_data(data,i), true);
		checksum+= ga_dev_xbee_frame_data(data,i);
	}

	// perform 2s complement
	checksum = 0xff - checksum;

	// send checksum
	ga_dev_xbee_send_byte(checksum, true);
}
