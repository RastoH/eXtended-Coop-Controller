extern "C" {
	#include <inttypes.h>
	#include <Math.h>
}

#define tl_second 0
#define tl_minute 1
#define tl_hour 2
#define tl_day 3
#define tl_month 4
#define tl_year 5

// Sunrise and Sunset DawnTsype
#define DAWN_NORMAL            1.585340737228125   //  90degree50' - 90.833 
#define DAWN_CIVIL             1.675516081914556   //  96degree             Twilight
#define DAWN_NAUTIC            1.780235837034216   // 102degree
#define DAWN_ASTRONOMIC        1.884955592153876   // 108degree

class TimeLord{
	public:
		TimeLord();
		double SUNRISE_DAWN_ANGLE;// = DAWN_NORMAL;  // DAWN_CIVIL   // Select desired Dawn Angle from (DAWN_NORMAL, DAWN_CIVIL, DAWN_NAUTIC, DAWN_ASTRONOMIC)
		// configuration
		bool Position(double/*float*/, double/*float*/);
		bool TimeZone(int);
		bool DstRules(uint8_t,uint8_t,uint8_t,uint8_t,uint8_t);
		double Angle(double); // Select desired Dawn Angle from (DAWN_NORMAL, DAWN_CIVIL, DAWN_NAUTIC, DAWN_ASTRONOMIC)
		
		// Political
		void GMT(uint8_t *);
		void DST(uint8_t *);
				
		//Solar & Astronomical
		bool SunRise(uint8_t *);
		bool SunSet(uint8_t *);
		double/*float*/ MoonPhase(uint8_t *);
		void Sidereal(uint8_t *, bool);
		uint8_t Season(uint8_t *);
		
		// Utility
		uint8_t DayOfWeek(uint8_t *);
		uint8_t LengthOfMonth(uint8_t *);
		bool IsLeapYear(int);
		
                // these were private
		void Adjust(uint8_t *, long);
		long DayNumber(uint16_t, uint8_t, uint8_t);
		bool InDst(uint8_t *);
	private:
		double/*float*/ latitude, longitude;
		int timezone;
		uint8_t dstm1, dstw1, dstm2, dstw2, dstadv;
		bool ComputeSun(uint8_t *, bool);
		char Signum(int);
		int Absolute(int);
		uint8_t _season(uint8_t *);
};
