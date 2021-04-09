#!/usr/bin/env perl6
use v6;

use RPi::Wiring::Pi;

#| The length of time the light panel should show "emergency" mode: 30 seconds.
constant QUIETUDE_DURATION = 30;

#| How often the light on the quiet button should blink on and off while in
#| quiet mode.
constant BLINK_FREQ        = 0.5;

class Pin {
    #= Object to manage the reading, writing, and monitoring the state of a GPIO
    #= pin on the Raspberry Pi

    #| This is the number of pin to read/write/monitor
    has $.pin is required;


    #| Sets the read/write mode. The $mode may be INPUT or OUTPUT.
    method mode($mode) { pinMode($.pin, $mode) }

    #| Read the immediate value of the pin. Returns HIGH or LOW.
    method read() { digitalRead($.pin) }

    #| Write the given state to the value of the pin. The $state may be HIGH or LOW.
    method write($state) { digitalWrite($.pin, $state) }

    #| Toggle the state from HIGH to LOW based on current state.
    method toggle() {
        if digitalRead($.pin) == HIGH {
            digitalWrite($.pin, LOW);
        }
        else {
            digitalWrite($.pin, HIGH);
        }
    }

    #| Return a Supply that emits changes to pin state. Emitted values are
    #| either HIGH or LOW.
    method events() {
        Supply.interval(0.01).map({
            state $prev;
            my $state = self.read;

            LEAVE { $prev = $state };

            if !$prev.defined || $prev != $state {
                $state;
            }
            else {
                Mu
            }
        }).grep({ .defined });
    }
}

#| Enumerate the three possible states of the on air light.
enum SignalState ( OffAir => 0, OnAir => 1, Quietude => 2 );

class LightPanel {
    #= Manages the state changes and messages sent to the light panel to switch
    #= it from one mode to the next.

    #| The IP address of the panel (or a broadcast address if I'm feeling
    #| careless.)
    has Str $.panel-ip is required;

    #| Flag that tells us whether I am testing or not.
    has Bool $.dry-run = False;

    #| The current state we want/assume the light panel is in.
    has SignalState $.signal-state = OffAir;

    #| The socket that sends our UDP packets.
    has IO::Socket::Async $!sock .= udp;

    #| Returns True when the on air light is showing either on air state.
    method is-on-air { $.signal-state != OffAir }

    #| Returns True when teh on air light is showing the "emergency" state.
    method is-quiet  { $.signal-state == Quietude }

    #| Send the UDP packet for going on air.
    method go-on-air {
        return if $.is-on-air;
        $!signal-state = OnAir;
        self.send-state-update();
    }

    #| Send the UDP packet to end "emergency" state.
    method end-quietude {
        return unless $.is-quiet;
        if $.is-on-air {
            $!signal-state = OnAir;
        }
        else {
            $!signal-state = OffAir;
        }
        self.send-state-update();
    }

    #| Send the UDP packet to go off air.
    method go-off-air {
        $!signal-state = OffAir;
        self.send-state-update();
    }

    #| Send the UDP packet to go into "emergency" state.
    method go-quiet {
        return unless $.is-on-air;
        $!signal-state = Quietude;
        self.send-state-update();
    }

    #| Send the actual packet (or log the dry run action).
    method send-state-update() {
        return self.dry-run-state-update() if $.dry-run;
        $!sock.write-to($!panel-ip, 10101, Blob.new([ $!signal-state.Int ]));
    }

    #| Log the dry run action.
    method dry-run-state-update() {
        say "Sending state: $.signal-state";
    }
}

class ControlPanel {
    #= Monitor the state of all buttons and switches on the control panel and
    #= provide feedback.

    constant ON_AIR_BUTTON = 3; #= Pin # of the on air switch
    constant QUIET_BUTTON  = 2; #= Pin # of the quiet button
    constant QUIET_LIGHT   = 0; #= Pin # of the light on the quiet button

    #| Pin object for the on air switch
    has $.on-air-button = Pin.new(:pin(ON_AIR_BUTTON));

    #| Pin object for the quiet button
    has $.quiet-button  = Pin.new(:pin(QUIET_BUTTON));

    #| Pin object for the light on the quiet button
    has $.quiet-light   = Pin.new(:pin(QUIET_LIGHT));

    #| Initialize the input/output mode of each pin and make sure the quiet
    #| button light is off.
    submethod TWEAK {
        $!on-air-button.mode(INPUT);
        $!quiet-button.mode(INPUT);
        $!quiet-light.mode(OUTPUT);

        $!quiet-light.write(LOW);
    }

    #| True if the on air switch is flipped on.
    method is-on-air() { $.on-air-button.read == HIGH }

    #| True if the quiet button is pressed.
    method is-quiet()  { $.quiet-button.read  == HIGH }

    #| A Supply that emits True whenever the on air switch is on and emits False
    #| otherwise.
    method on-air-events() { $.on-air-button.events.map(* == HIGH) }

    #| A Supply that emits True whenever the quiet button is on and emits False
    #| otherwise.
    method quiet-events() { $.quiet-button.events.map(* == HIGH) }
}

class Blinker {
    #= While quiet mode is engaged, the quiet light is set to blink. This object
    #= keeps the light blinking.

    has Pin $.light is required; #= Pin object of the light on the quiet button.
    has Tap $.blinky-blinky;     #= The tap that toggles the light on the quiet button.

    #| Causes the light on the quiet button to start blinking on and off every
    #| half second.
    method start {
        with $!blinky-blinky {
            $!blinky-blinky.close;
            $!blinky-blinky = Nil;
        }

        $!blinky-blinky = Supply.interval(BLINK_FREQ).tap({ $.light.toggle });
    }

    #| Causes the light on the quiet button to turn off and stop blinking.
    method stop {
        with $!blinky-blinky {
            $!blinky-blinky.close;
            $!blinky-blinky = Nil;
            $.light.write(LOW);
        }
    }
}

#| Monitor and manage the control panel for my on air light.
sub MAIN(
    :$dry-run = False, #= Set this flag to enter debugging mode
    :$panel-ip!,       #= Set this to the IP/broadcast address of the light panel
) {
    # Initialize the Raspberry Pi library
    wiringPiSetup();

    # Initialize our objects for managing the control panel pieces
    my LightPanel   $light         .= new(:$dry-run, :$panel-ip);
    my ControlPanel $control       .= new;
    my Blinker      $quiet-blinker .= new(light => $control.quiet-light);

    # When we go to quiet/"emergency" state, this promise will be initialized.
    # If the on air switch is turned off early, we keep this promise to trigger
    # an early end to the quiet/"emergency" state.
    my Promise      $early-quiet;

    react {

        # Monitor the on air switch
        whenever $control.on-air-events {

            # When the on air switch is flipped on, switch the light panel to on
            # air mode.
            when True {
                $light.go-on-air;
            }

            # When the on air switch is flipped off, make sure the quiet blink
            # is off and make sure the light panel is off air.
            when False {
                $light.go-off-air;

                # Also keep the $early-quiet promise, if present
                .keep with $early-quiet;
            }
        }

        # Monitor the quiet button
        whenever $control.quiet-events {

            # When the quiet button is pressed and the control is already in on
            # air mode...
            when True {
                if $control.is-on-air {
                    $quiet-blinker.start;

                    # Stop blinking and turn off "emergency" mode in 30 seconds
                    # or when the on air switch goes off early.
                    Promise.anyof(
                        $early-quiet = Promise.new,
                        Promise.in(QUIETUDE_DURATION),
                    ).then({
                        $early-quiet = Nil;
                        $light.end-quietude;
                        $quiet-blinker.stop;
                    });

                    # Turn the light panel to "emergency" mode
                    $light.go-quiet;
                }
            }

            # As the quiet button is toggle on only, we do nothing when the
            # button is released.
        }
    }
}
