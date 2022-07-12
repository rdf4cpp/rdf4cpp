# Working Nature of tm and mktime()

tm_sec, tm_min and tm_hours represent seconds, minutes and hours of a time respectively. tm_year, tm_mon and tm_mday represent year, month and day of a date respectively. All values should be in the range as specified below.
isdst represents whether daylight saving is enabled or disabled. It is advised to set to -1 for proper calculations of xsd datatypes.

    struct tm tm{};
    tm.tm_sec = 0; // seconds of minutes from 0 to 60
    tm.tm_min = 0; // minutes of hour from 0 to 59
    tm.tm_hour = 0; // hours of day from 0 to 24
    tm.tm_year = 2022 - 1900; // year since 1900
    tm.tm_mon = 4 - 1; // month of year from 0 to 11
    tm.tm_mday = 31; // day of the month 0 to 31 depending on month
    tm.tm_isdst = -1; // value should be set even if not used

mktime() converts the values of struct to seconds and also rearranges any out of range values of its members.

For Date all the value except tm_year, tm_mon, and tm_mday should be set as below to avoid possible errors in conversion.
    
    struct tm tm{};
    tm.tm_sec = 0; // seconds of minutes from 0 to 60
    tm.tm_min = 0; // minutes of hour from 0 to 59
    tm.tm_hour = 0; // hours of day from 0 to 24
    tm.tm_year = 2022 - 1900; // year since 1900
    tm.tm_mon = 4 - 1; // month of year from 0 to 11
    tm.tm_mday = 31; // day of the month 0 to 31 depending on month
    tm.tm_isdst = -1; // value should be set even if not used
    value = mktime(&tm);

The initalized tm_mday value is not range because April 2022 only has 30 days. Invocation of mktime() rearranges this to make the date in range. So, the resulting tm will be as below and the resulting date will be 01-05-2022 (May 1 2022).

    struct tm tm{};
    tm.tm_sec = 0; // seconds of minutes from 0 to 60
    tm.tm_min = 0; // minutes of hour from 0 to 59
    tm.tm_hour = 0; // hours of day from 0 to 24
    tm.tm_year = 2022 - 1900; // year since 1900
    tm.tm_mon = 5 - 1; // month of year from 0 to 11
    tm.tm_mday = 1; // day of the month 0 to 31 depending on month
    tm.tm_isdst = -1; // value should be set even if not used

# Using time Library for Time and Date Datatypes

Initialization can be done using struct tm, which consist of members of date and time fragments. Below are the default values that need to be set:

For Date all the value except tm_year, tm_month, and tm_mday should be set as below to avoid possible errors in conversion.
    
    struct tm tm{};
    tm.tm_sec = 0; // seconds of minutes from 0 to 60
    tm.tm_min = 0; // minutes of hour from 0 to 59
    tm.tm_hour = 0; // hours of day from 0 to 24
    tm.tm_year = 2022 - 1900; // year since 1900
    tm.tm_mon = 4 - 1; // month of year from 0 to 11
    tm.tm_mday = 22; // day of the month 0 to 31 depending on month
    tm.tm_isdst = -1; // value should be set even if not used
    value = mktime(&tm);


For Time all the value except tm_sec, tm_min, and tm_hour should be set as below to avoid possible errors during conversion. 
If tm_mday set to 0 when hour < 24 this implies a date of any year < 1900. As time library uses date since 01-01-1900, this can leads to tm_year finally set to negative. This will affect the calculations. 

    struct tm tm{};
    tm.tm_sec = 30; // seconds of minutes from 0 to 60
    tm.tm_min = 15; // minutes of hour from 0 to 59
    tm.tm_hour = 10; // hours of day from 0 to 24
    tm.tm_year = 0; // year since 1900
    tm.tm_mon = 0; // month of year from 0 to 11
    tm.tm_mday = 1; //date of the month to be set to 0, if tm_hour > 23
    tm.tm_isdst = -1; // value should be set even if not used
    value = mktime(&tm);

An exceptional case in time : value of tm_mday to be set to 0 if one needs to set tm_hour > 23

    struct tm tm{};
    tm.tm_sec = 0; // seconds of minutes from 0 to 60
    tm.tm_min = 0; // minutes of hour from 0 to 59
    tm.tm_hour = 24; // hours of day from 0 to 24
    tm.tm_year = 0; // year since 1900
    tm.tm_mon = 0; // month of year from 0 to 11
    tm.tm_mday = 0; //date of the month to be set to 0, if tm_hour > 23
    tm.tm_isdst = -1; // value should be set even if not used
    value = mktime(&tm);
