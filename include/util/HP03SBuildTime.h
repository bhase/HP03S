#ifndef D_HP03SBuildTime_H
#define D_HP03SBuildTime_H

///////////////////////////////////////////////////////////////////////////////
//
//  HP03SBuildTime is responsible for recording and reporting when
//  this project library was built
//
///////////////////////////////////////////////////////////////////////////////

class HP03SBuildTime
  {
  public:
    explicit HP03SBuildTime();
    virtual ~HP03SBuildTime();
    
    const char* GetDateTime();

  private:
      
    const char* dateTime;

    HP03SBuildTime(const HP03SBuildTime&);
    HP03SBuildTime& operator=(const HP03SBuildTime&);

  };

#endif  // D_HP03SBuildTime_H
