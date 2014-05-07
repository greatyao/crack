#include "CompClient.h"

CCompClient::CCompClient(void)
{
	m_gputhreads = -1;
	m_cputhreads = -1;
}

CCompClient::~CCompClient(void)
{
}

void CCompClient::SetCPUGPU(int cpu, int gpu)
{
	m_gputhreads = gpu;
	m_cputhreads = cpu;
}