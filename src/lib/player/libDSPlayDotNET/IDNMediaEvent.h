#pragma once
#pragma unmanaged

#pragma managed



namespace illiminable {
namespace libDSPlayDotNET {

public __gc interface IDNMediaEvent {

public:
	virtual bool eventNotification(Int32 inEventCode, Int32 inParam1, Int32 inParam2) = 0;


};

}
}