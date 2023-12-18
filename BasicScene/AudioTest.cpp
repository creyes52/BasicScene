#include <initguid.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <stdio.h>
#include <iostream>

// REFERENCE_TIME time units per second and per millisecond
#define REFTIMES_PER_SEC       10000000
#define REFTIMES_PER_MILLISEC     10000

#define EXIT_ON_ERROR2(hres, message)  \
              if (FAILED(hres)) { goto Exit; } else { printf("- OK %s\n", message); }

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }

#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

HRESULT Test() {
	HRESULT hr;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC/100;
	REFERENCE_TIME hnsActualDuration;
	UINT32 bufferFrameCount;
	UINT32 numFramesAvailable;
	IMMDevice* pDevice = NULL;
	IAudioClient* pAudioClient = NULL;
	IMMDeviceEnumerator *pEnumerator = NULL;
	IAudioCaptureClient* pCaptureClient = NULL;
	WAVEFORMATEX* pwfx = NULL;
	UINT32 packetLength = 0;
	BOOL bDone = FALSE;
	BYTE* pData;
	DWORD flags;

	CoInitialize(NULL);
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**) &pEnumerator);
	EXIT_ON_ERROR2(hr, "CoCreateInstance()");

	hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice);
	EXIT_ON_ERROR2(hr, "GetDefaultAudioEndpoint()");

	hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient);
	EXIT_ON_ERROR2(hr, "pDevice->Activate()");
	LPWSTR strId;
	pDevice->GetId(&strId);
	std::wcout << "Device Id: " << strId << "\n";

	hr = pAudioClient->GetMixFormat(&pwfx);
	EXIT_ON_ERROR2(hr, "pAudioClient->GetMixFormat()");

	hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, hnsRequestedDuration, 0, pwfx, NULL);
	EXIT_ON_ERROR2(hr, "pAudioClient->Initialize()");

	// Get the size of the allocated buffer.
	hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	EXIT_ON_ERROR2(hr, "pAudioClient->GetBufferSize()")
	printf("Buffer frame: %i\n", bufferFrameCount);

	hr = pAudioClient->GetService(IID_IAudioCaptureClient, (void**)&pCaptureClient);
	EXIT_ON_ERROR2(hr, "GetService()")

	// Calculate the actual duration of the allocated buffer.
	hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / pwfx->nSamplesPerSec;
	printf("Buffer duration: %I64d\n", hnsActualDuration);
	
	hr = pAudioClient->Start();  // Start recording.
	EXIT_ON_ERROR2(hr, "pAudioClient->Start()")

	// Each loop fills about half of the shared buffer.
	while (bDone == FALSE)
	{
		// Sleep for half the buffer duration.
		Sleep(hnsActualDuration / REFTIMES_PER_MILLISEC / 2);

		hr = pCaptureClient->GetNextPacketSize(&packetLength);
		EXIT_ON_ERROR(hr)

		while (packetLength != 0)
		{
			// Get the available data in the shared buffer.
			hr = pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL);
			EXIT_ON_ERROR(hr)
			printf("B");

			if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
			{
				pData = NULL;  // Tell CopyData to write silence.
			}

			// Copy the available capture data to the audio sink.
			//hr = pMySink->CopyData(pData, numFramesAvailable, &bDone);
			EXIT_ON_ERROR(hr)

			hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
			EXIT_ON_ERROR(hr)

			hr = pCaptureClient->GetNextPacketSize(&packetLength);
			EXIT_ON_ERROR(hr)
		}
	}

Exit:
	CoTaskMemFree(pwfx);
	SAFE_RELEASE(pEnumerator)
	SAFE_RELEASE(pDevice)
	SAFE_RELEASE(pAudioClient)
	SAFE_RELEASE(pCaptureClient)

	return hr;
}