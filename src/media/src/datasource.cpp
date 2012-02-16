#include "datasource.h"

namespace media {

DataSource::DataSource()
{
}

DataSource::~DataSource()
{
}

long long DataSource::Available()
{
	return 0LL;
}

long long DataSource::GetCapacity()
{
	return 0LL;
}

void DataSource::Open()
{
}

void DataSource::Reset()
{
}

int DataSource::AddData(char *data, int size)
{
	return 0;
}

int DataSource::GetData(char *data, int size)
{
	return 0;
}

}
