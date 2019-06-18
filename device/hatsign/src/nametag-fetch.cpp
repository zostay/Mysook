#include "nametag-fetch.h"
#include "secrets.h"

void NTOHL(uint32_t &value) {
    uint32_t tmp_a = (value & 0xff000000) >> 0x18; \
    uint32_t tmp_b = (value & 0x00ff0000) >> 0x08; \
    uint32_t tmp_c = (value & 0x0000ff00) << 0x08; \
    uint32_t tmp_d = (value & 0x000000ff) << 0x18; \
    value = tmp_d | tmp_c |tmp_b | tmp_a; \
}

void NameTagFetch::clear_programs() {
    for (int i = 0; i < length; i++) {
        programs[i]->refdec();
        if (programs[i]->get_refcount() == 0)
            delete programs[i];
    }
    length = 0;
    index = 0;
}

void NameTagFetch::add_program(WebProgramInfo *program) {
    if (length >= 32) {
        log.logf_ln("E [nametag-fetch] too many programs loaded, skipping program add");
        delete program;
        return;
    }

    programs[length++] = program;
}

bool NameTagFetch::readd_program(int program_id) {
    if (length >= 32) {
        log.logf_ln("E [nametag-fetch] too many porgrams loaded, skipping program re-add");
        return false;
    }

    WebProgramInfo *found = 0;
    for (int i = 0; i < length; i++) {
        if (programs[i]->get_program_id() == program_id) {
            found = programs[i];
            break;
        }
    }

    if (found) {
        found->refinc();
        programs[length++] = found;
        return true;
    }

    return false;
}

bool NameTagFetch::fetch_queue(const char *queue_name) {
    clear_programs();

    String queue_url = String(base_url) 
                     + String("/queue/") 
                     + String(queue_name);

    HTTPClient ua;
    WiFiClient *client;
    if (String(base_url).startsWith(String("https:"))) {
        WiFiClientSecure *secure_client = new WiFiClientSecure;
        secure_client->setCACert(ca_cert);
        client = secure_client;
    }
    else {
        client = new WiFiClient;
    }

    ua.begin(*client, queue_url);

    int code;
    if ((code = ua.GET()) <= 0) {
        log.logf_ln("E [nametag-fetch] unable to fetch nametag queue %s", queue_name);
        return false;
    }

    if (code != 200) {
        log.logf_ln("E [nametag-fetch] unexpected HTTP status %d when fetching nametag queue %s", queue_name);
        return false;
    }

    String payload = ua.getString();
    auto error = deserializeJson(qdata, payload);
    if (error) {
        log.logf_ln("E [nametag-fetch] unable to read JSON %s", error.c_str());
        return false;
    }

    JsonArray programs = qdata["programs"].as<JsonArray>();
    for (JsonVariant program : programs) {
        int program_id        = program["id"].as<int>();

        if (readd_program(program_id)) {
            log.logf_ln("E [nametag_fetch] Re-adding program %d", program_id);
            continue;
        }

        String program_name   = program["name"].as<String>();
        String program_author = program["author"].as<String>();

        String program_url = String(base_url)
                           + String("/program/")
                           + String(program_id)
                           + String("/descriptor");

        log.logf_ln("I [nametag-fetch] Fetching program %d", program_id);

        ua.begin(*client, program_url);
        ua.addHeader("Authorization", admin_token);

        if ((code = ua.GET()) <= 0) {
            log.logf_ln("E [nametag-fetch] unable to fetch nametag program %d (%d): %s", program_id, code, ua.errorToString(code).c_str());
            return false;
        }

        if (code != 200) {
            log.logf_ln("E [nametag-fetch] unexpected HTTP status %d when fetching nametag program %d", code, program_id);
            return false;
        }

        // Program is too long
        int byte_length = ua.getSize();
        if (byte_length >= 32768) {
            log.logf_ln("E [nametag-fetch] program %d is too long, will not load", program_id);
            continue;
        }

        // Program must be made of long words
        if (byte_length % 4 != 0) {
            log.logf_ln("E [nametag-fetch] program %d fetch error, program contains partial long words", program_id);
            continue;
        }

        WebProgramInfo *web_binary = new WebProgramInfo(
            program_id,
            program_name,
            program_author,
            3 + byte_length / 4
        );

        union {
            uint8_t buffer[4];
            uint32_t word;
        } hp;

        //uint8_t byte_payload[32768];
        WiFiClient &stream = ua.getStream();
        for (int i = 0; i < byte_length; i++) {
            if (stream.available()) {
                int byte_status = stream.read();
                if (byte_status < 0) {
                    log.logf_ln("Shorted a byte at %d", i);
                    return false;
                }

                //log.logf("%c", byte_status);

                hp.buffer[i % 4] = byte_status;
                if ((i+1) % 4 == 0) {
                    NTOHL(hp.word);
                    //log.logf("%08x ", hp.word);
                    web_binary->write(hp.word);
                }
            }
        }

        // add the program terminator
        web_binary->write(0xDEAD);
        web_binary->write(0xDEAD);
        web_binary->write(0xDEAD);

        //log.logf("%08x ", 0xDEAD);
        //log.logf("%08x ", 0xDEAD);
        //log.logf("%08x ", 0xDEAD);
        //log.logf_ln("");

        web_binary->lock();
        add_program(web_binary);

        log.logf_ln("I [nametag-fetch] Adding #%d: %s (%s)",
            web_binary->get_program_id(),
            web_binary->get_name().c_str(),
            web_binary->get_author().c_str()
        );
    }

    ua.end();
    delete client;

    return true;
}

