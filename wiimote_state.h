// _______________________________________________________________________________
//
//	 - WiiYourself! - native C++ Wiimote library  v1.14 BETA
//	  (c) gl.tter 2007-9 - http://gl.tter.org
//
//	  see License.txt for conditions of use.  see History.txt for change log.
// _______________________________________________________________________________
//
//  wiimote_state.h  (tab = 4 spaces)

// the 'wiimote_state' struct contains all the Wiimote and Extension state data
//  (buttons etc) - the wiimote class inherits from this and the app can poll
//  the data there at any time.
#ifdef _MSC_VER // VC
# pragma once
#endif

#ifndef _WIIMOTE_STATE_H
# define _WIIMOTE_STATE_H

// speaker support:
enum speaker_freq
	{
	// (keep in sync with FreqLookup in wiimote.cpp)
	FREQ_NONE	= 0,
	// my PC can't keep up with these using bUseHIDwrite, so I haven't
	//  been able to tune them yet
	FREQ_4200HZ = 1,
	FREQ_3920HZ = 2,
	FREQ_3640HZ = 3,
	FREQ_3360HZ = 4,
	// these were tuned until the square-wave was glitch-free on my remote -
	//  may not be exactly right
	FREQ_3130HZ = 5,	// +190
	FREQ_2940HZ = 6,	// +180
	FREQ_2760HZ = 7,	// +150
	FREQ_2610HZ = 8,	// +140
	FREQ_2470HZ = 9,
	};

// wiimote_sample - holds the audio sample in the native wiimote format
struct wiimote_sample
	{
	wiimote_sample() : samples(NULL), length(0), freq(FREQ_NONE) {}
	BYTE*		 samples;
	DWORD		 length;
	speaker_freq freq;
	};

// flags & masks that indicate which part(s) of the wiimote state have changed
enum state_change_flags
	{
	// state didn't change at all
	NO_CHANGE					   = 0,

	// Wiimote specific:
	CONNECTION_LOST				   = 1<<0,
	BATTERY_CHANGED				   = 1<<1,
	BATTERY_DRAINED				   = 1<<2, // close to empty
	LEDS_CHANGED				   = 1<<3, // (probably redudant as wiimmote never
	BUTTONS_CHANGED				   = 1<<4, //  changes them unless requested)
	ACCEL_CHANGED				   = 1<<5,
	ORIENTATION_CHANGED			   = 1<<6,
	IR_CHANGED					   = 1<<7,
	//  all wiimote flags
	WIIMOTE_CHANGED				   = CONNECTION_LOST|BATTERY_CHANGED|BATTERY_DRAINED|
								     LEDS_CHANGED|BUTTONS_CHANGED|ACCEL_CHANGED|
								     ORIENTATION_CHANGED|IR_CHANGED,
	// - Extensions -:
	//  Nunchuk:
	NUNCHUK_CONNECTED			   = 1<<8,
	NUNCHUK_BUTTONS_CHANGED		   = 1<<9,
	NUNCHUK_ACCEL_CHANGED		   = 1<<10,
	NUNCHUK_ORIENTATION_CHANGED	   = 1<<11,
	NUNCHUK_JOYSTICK_CHANGED	   = 1<<12,
	//   all flags
	NUNCHUK_CHANGED				   = NUNCHUK_CONNECTED|NUNCHUK_BUTTONS_CHANGED|
								     NUNCHUK_ACCEL_CHANGED|NUNCHUK_ORIENTATION_CHANGED|
								     NUNCHUK_JOYSTICK_CHANGED,
	//  Classic Controller (inc. Guitars etc):
	CLASSIC_CONNECTED			   = 1<<13,
	CLASSIC_BUTTONS_CHANGED		   = 1<<14,
	CLASSIC_JOYSTICK_L_CHANGED	   = 1<<15,
	CLASSIC_JOYSTICK_R_CHANGED	   = 1<<16,
	CLASSIC_TRIGGERS_CHANGED	   = 1<<17,
	//   all flags
	CLASSIC_CHANGED				   = CLASSIC_CONNECTED|CLASSIC_BUTTONS_CHANGED|
								     CLASSIC_JOYSTICK_L_CHANGED|
								     CLASSIC_JOYSTICK_R_CHANGED|
									 CLASSIC_TRIGGERS_CHANGED,
	//  Balance Board:
	BALANCE_CONNECTED			   = 1<<18,
	BALANCE_WEIGHT_CHANGED		   = 1<<19,
	//   all flags
	BALANCE_CHANGED				   = BALANCE_CONNECTED|BALANCE_WEIGHT_CHANGED,

	//  Motion Plus
	MOTIONPLUS_DETECTED			   = 1<<20, // attached but not enabled
	MOTIONPLUS_ENABLED			   = 1<<21,
	MOTIONPLUS_SPEED_CHANGED	   = 1<<22,
	MOTIONPLUS_EXTENSION_CONNECTED = 1<<23,		// an extension is found in the
												//  MotionPlus port
	MOTIONPLUS_EXTENSION_DISCONNECTED = 1<<24, // it was disconnected
	//   all flags
	MOTIONPLUS_CHANGED			   = MOTIONPLUS_DETECTED|MOTIONPLUS_ENABLED|
									 MOTIONPLUS_SPEED_CHANGED|
									 MOTIONPLUS_EXTENSION_CONNECTED|
									 MOTIONPLUS_EXTENSION_DISCONNECTED,
	//  General:
	EXTENSION_DISCONNECTED		 = 1<<25,
	EXTENSION_PARTIALLY_INSERTED = 1<<26,
	EXTENSION_CONNECTED			 = NUNCHUK_CONNECTED|CLASSIC_CONNECTED|
								   BALANCE_CONNECTED|MOTIONPLUS_ENABLED,
	EXTENSION_CHANGED			 = EXTENSION_DISCONNECTED|NUNCHUK_CHANGED|
								   CLASSIC_CHANGED|BALANCE_CHANGED|MOTIONPLUS_CHANGED,
	//  ALL flags:
	CHANGED_ALL					 = WIIMOTE_CHANGED|EXTENSION_CHANGED,
	};

// wiimote_state (contains the Wiimote and Extension data and settings)
struct wiimote_state
	{
	friend class wiimote; // for Clear()

	// calibration information (stored on the Wiimote)
	struct calibration_info
		{
		BYTE X0, Y0, Z0;
		BYTE XG, YG, ZG;
		} CalibrationInfo;

	// button state:
	struct buttons
		{
		// convenience accessors
		inline bool A		() const	{ return (Bits & _A)    != 0; }
		inline bool B		() const	{ return (Bits & _B)	!= 0; }
		inline bool Plus	() const	{ return (Bits & PLUS)  != 0; }
		inline bool Home	() const	{ return (Bits & HOME)  != 0; }
		inline bool Minus	() const	{ return (Bits & MINUS) != 0; }
		inline bool One		() const	{ return (Bits & ONE)   != 0; }
		inline bool Two		() const	{ return (Bits & TWO)   != 0; }
		inline bool Up		() const	{ return (Bits & UP)	!= 0; }
		inline bool Down	() const	{ return (Bits & DOWN)  != 0; }
		inline bool Left	() const	{ return (Bits & LEFT)  != 0; }
		inline bool Right	() const	{ return (Bits & RIGHT) != 0; }

		// all 11 buttons stored as bits (set = pressed)
		WORD Bits;

		// button bit masks (little-endian order)
		enum mask
			{
			LEFT	= 0x0001,
			RIGHT	= 0x0002,
			DOWN	= 0x0004,
			UP		= 0x0008,
			PLUS	= 0x0010,
			TWO		= 0x0100,
			ONE		= 0x0200,
			_B		= 0x0400,	// ie. trigger
			_A		= 0x0800,
			MINUS	= 0x1000,
			HOME	= 0x8000,
			//
			ALL		= LEFT|RIGHT|DOWN|UP|PLUS|TWO|ONE|_A|_B|MINUS|HOME,
			};
		} Button;

	// accelerometers state:
	struct acceleration
		{
		BYTE  RawX, RawY, RawZ;
		float X, Y, Z;

		// note: experimental! the orientation values can only be safely estimated
		//        if the controller isn't accelerating (otherwise there is no
		//		  simple way to seperate orientation from acceleration - except
		//		  perhaps using the IR reference and/or some clever assumptions).
		//		  so for now the code only updates orientation if the controller
		//		  appear to be stationary (by checking if the acceleration vector
		//		  length is near 1G for several updates in a row).
		//		  also note that there is no way to detect Yaw from the accelerometer
		//		  alone when it's pointing at the screen (and I'm not curently
		//		  processing IR):
		struct orientation
			{
			float X, Y, Z;
			unsigned UpdateAge;	// how many acceleration updates ago the last
								//  orientation estimate was made (if this
								//  value is high, the values are out-of-date
								//  and probably shouldn't be used).
			// Euler angle support (useful for some things).
			// * note that decomposing to Euler angles is complex, not always reliable,
			//    and also depends on your assumptions about the order each component
			//    is applied in.  you may need to handle this yourself for more
			//    complex scenarios *
			float	 Pitch;	// in degrees (-180 - +180)
			float	 Roll;	// "
			// float Yaw;	
			} Orientation;
		} Acceleration;

	// IR camera state:
	struct ir
		{
		// in theory the IR imager is 1024x768 and so should report raw coords
		//  0-1023 x 0-767.  in practice I have never seen them exceed the values
		//  below, so I'm using them instead to give the full 0-1 float range
		//  (it's possible that the edge pixels are used for processing, or masked
		//  out due to being unreliable).  let me know if your wiimote reports
		//  a different rage.
		static const unsigned MAX_RAW_X = 1016;
		static const unsigned MAX_RAW_Y = 760;

		// data mode reported by the IR sensor
		enum mode
			{
			OFF			= 0x00,
			BASIC		= 0x01,	// 10 bytes
			EXTENDED	= 0x03,	// 12 bytes
			FULL		= 0x05,	// 16 bytes * 2 (format unknown)
			};
		
		mode	Mode;		// read-only (depends on ReportType set)
		
		struct dot
			{
			bool     bVisible;	// other values are not valid if == false
			unsigned RawX;
			unsigned RawY;
			float    X;			// 0-1 (left-right)
			float    Y;			// "   (top -bottom)
			int		 Size;		// (not available in BASIC mode)
			} Dot[4];
		} IR;

	struct leds
		{
		// all LEDs stored in bits 0-3  (1 = lit)
		 BYTE Bits;

		// convenience accessors:
		inline bool Lit (unsigned index)
			{ _ASSERT(index < 4);
			  return (index >= 4)? false : ((Bits & (1<<index)) != 0); }
		} LED;

	 BYTE BatteryRaw;		// 0 - ~200 (it seems 200 *may* be the maximum charge)
	 BYTE BatteryPercent;	// (using the above assumption, where 200 raw = 100%)
	 bool bBatteryDrained;	// battery is nearly flat
	 bool bRumble;
	 bool bExtension;		// an extension (eg. Nunchuk) is connected.

	// speaker state:
	struct speaker
		{
		 bool		  bEnabled;
		 bool		  bMuted;
		 speaker_freq Freq;
		 BYTE		  Volume;
		} Speaker;

	// the extension plugged into the Wiimote (if any)
	enum extension_type
		{
		NONE = 0,
		NUNCHUK,
		CLASSIC,
		GH3_GHWT_GUITAR, // GH3 or GHWT Guitar (treated as Classic)
		GHWT_DRUMS,		 // not yet used
		BALANCE_BOARD,
		MOTION_PLUS,
		PARTIALLY_INSERTED,
		};
	 extension_type	ExtensionType;

	// joystick struct (shared between Nunchuk & Classic Controller)
	struct joystick
		{
		friend class wiimote;
		
		// raw unprocessed coordinates:
		float RawX, RawY;

		// processed coordinates in approximately -1 - +1 range (includes calibration
		//  data and deadzones) - note that due to calibration inaccuracies, the
		//  extremes may be slightly over/under (+-)1.0.
		float X, Y;

		// a 'deadzone' is a user-defined range near the joystick center which
		//  is treated as zero (joysticks often drift a little even at the center
		//  position).  you can set a deadzone for each axis at any time, range is
		//  0.0 (off) to 1.0 (entire range - not useful :). try 0.03.
		struct deadzone
			{
			 float X, Y;
			} DeadZone;
		};

	// Nunchuk state (if connected)
	struct nunchuk
		{
		struct calibration_info
			{
			 BYTE X0, Y0, Z0;
			 BYTE XG, YG, ZG;
			 BYTE MinX, MidX, MaxX;
			 BYTE MinY, MidY, MaxY;
			} CalibrationInfo;

		acceleration  Acceleration;
		joystick	  Joystick;
		 bool C;
		 bool Z;
		} Nunchuk;

	// Classic Controller state (if connected)
	struct classic_controller
		{
		// calibration information (stored on the controller)
		struct calibration_info
			{
			 BYTE MinXL, MidXL, MaxXL;
			 BYTE MinYL, MidYL, MaxYL;
			 BYTE MinXR, MidXR, MaxXR;
			 BYTE MinYR, MidYR, MaxYR;
			 BYTE MinTriggerL, MaxTriggerL;
			 BYTE MinTriggerR, MaxTriggerR;
			} CalibrationInfo;

		// button state
		struct buttons
			{
			// convenience accessors
			inline  bool A			() const { return (Bits & _A)	  != 0; }
			inline  bool B			() const { return (Bits & _B)	  != 0; }
			inline  bool Plus		() const { return (Bits & PLUS)   != 0; }
			inline  bool Minus		() const { return (Bits & MINUS)  != 0; }
			inline  bool Home		() const { return (Bits & HOME)   != 0; }
			inline  bool Up			() const { return (Bits & UP)     != 0; }
			inline  bool Down		() const { return (Bits & DOWN)   != 0; }
			inline  bool Left		() const { return (Bits & LEFT)   != 0; }
			inline  bool Right		() const { return (Bits & RIGHT)  != 0; }
			inline  bool X			() const { return (Bits & _X)	  != 0; }
			inline  bool Y			() const { return (Bits & _Y)	  != 0; }
			inline  bool ZL			() const { return (Bits & _ZL)	  != 0; }
			inline  bool ZR			() const { return (Bits & _ZR)	  != 0; }
			inline  bool TriggerL	() const { return (Bits & TRIG_L) != 0; }
			inline  bool TriggerR	() const { return (Bits & TRIG_R) != 0; }
			
			// all 15 buttons stored as bits (set = pressed)
			 WORD Bits;

			// button bitmasks (little-endian order)
			enum mask
				{
				TRIG_R  = 0x0002,
				PLUS    = 0x0004,
				HOME    = 0x0008,
				MINUS   = 0x0010,
				TRIG_L  = 0x0020,
				DOWN    = 0x0040,
				RIGHT   = 0x0080,
				UP      = 0x0100,
				LEFT    = 0x0200,
				_ZR     = 0x0400,
				_X      = 0x0800,
				_A      = 0x1000,
				_Y      = 0x2000,
				_B      = 0x4000,
				_ZL     = 0x8000,
				//
				ALL		= TRIG_R|PLUS|HOME|MINUS|TRIG_L|DOWN|RIGHT|UP|LEFT|
						  _ZR|_X|_A|_Y|_B|_ZL,
				};
			} Button;

		// joysticks
		joystick JoystickL;
		joystick JoystickR;

		// triggers
		BYTE  RawTriggerL, RawTriggerR;
		float TriggerL, TriggerR;
		} ClassicController;

	struct balance_board
		{
		// values for each of the board's 4 sensors:
		//  (these values are always exposed unmodifed)
		struct sensors_raw
			{
			short	 TopR;
			short	 TopL;
			short BottomR;
			short BottomL;
			};
		struct sensors_f
			{
			float	 TopL;
			float	 TopR;
			float BottomL;
			float BottomR;
			
			float Total; // all 4 combined
			};

		// calibration info
		struct calibration_info
			{
			sensors_raw Kg0;	// calibration at  0 Kg
			sensors_raw Kg17;	// "			  17 Kg
			sensors_raw Kg34;	// "			  34 Kg
			} CalibrationInfo;

		// state:
		sensors_raw	Raw;	  // raw values (per sensor)
		sensors_f	AtRestKg; // set by Connect() and CalibrateAtRest()
		//  (the values below have their 'at-rest' offsets automatically removed)
		sensors_f	Kg;		  // kilograms  (per sensor)
		sensors_f	Lb;		  // pounds	  (per sensor)
		} BalanceBoard;

	struct motion_plus
		{
		//  (these values are always exposed unmodifed)
		struct sensors_raw
			{
			short Yaw;
			short Pitch;
			short Roll;
			};
		struct sensors_f
			{
			float Yaw;
			float Pitch;
			float Roll;
			};

		// state:
		sensors_raw	Raw;
		sensors_f	Speed;
		} MotionPlus;

	// ---- internal use only ----
	protected: 
		 unsigned WiimoteNearGUpdates;
		 unsigned NunchukNearGUpdates;

		void Clear (bool including_deadzones)
			{
			joystick::deadzone nunchuk_deadzone,
							   classic_joyl_deadzone,
							   classic_joyr_deadzone;
		
			// preserve the deadzone settings?
			if(!including_deadzones) {
				nunchuk_deadzone	  = Nunchuk.Joystick.DeadZone;
				classic_joyl_deadzone = ClassicController.JoystickL.DeadZone;
				classic_joyr_deadzone = ClassicController.JoystickR.DeadZone;
				}

			memset(this, 0, sizeof(wiimote_state));

			// restore the deadzones?
			if(!including_deadzones) {
				Nunchuk.Joystick.DeadZone			 = nunchuk_deadzone;
				ClassicController.JoystickL.DeadZone = classic_joyl_deadzone;
				ClassicController.JoystickR.DeadZone = classic_joyr_deadzone;
				}
			}
	};

#endif // _WIIMOTE_STATE_H