#include "demuxdatasource.h"

namespace media {

DemuxDataSource::DemuxDataSource(int apid, int vpid):
	DataSource()
{
	_apid = apid;
	_vpid = vpid;
}

DemuxDataSource::~DemuxDataSource()
{
}

long long DemuxDataSource::Available()
{
	return 0LL;
}

long long DemuxDataSource::GetCapacity()
{
	return 0LL;
}

void DemuxDataSource::Open()
{
}

void DemuxDataSource::Reset()
{
}

int DemuxDataSource::AddData(char *data, int size)
{
	return 0;
}

int DemuxDataSource::GetData(char *data, int size)
{
	return 0;
}

}
