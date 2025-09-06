#include "Dx12.h"
#include "Render.h"
#include "Menu.h"


#include "Fonts.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//#define IS_DEBUG_VERSION

//#define IMGUI_BLUR_TEST


namespace DX12
{
	Present12 oPresent = nullptr;
	CreateCommandQueue oCreateCommandQueue = nullptr;
	CreateSwapChain oCreateSwapChain = nullptr;
	uint64_t* MethodsTable = nullptr;
	PFN_D3D12CreateDevice oD3D12CreateDevice = nullptr;

	std::unordered_map<IDXGISwapChain*, ID3D12CommandQueue*> g_SwapChainToQueue;

	char dlldir[512] = { 0 };
	WNDCLASSEX WindowClass = { 0 };
	HWND WindowHwnd = nullptr;
	bool isOverlayReady = false;
	bool ImGui_Initialised = false;

	namespace Process {
		DWORD ID = 0;
		HANDLE Handle = nullptr;
		HWND Hwnd = nullptr;
		HMODULE Module = nullptr;
		WNDPROC WndProc = nullptr;
		int WindowWidth = 0;
		int WindowHeight = 0;
		LPCSTR Title = nullptr;
		LPCSTR ClassName = nullptr;
		LPCSTR Path = nullptr;
	}

	namespace DirectX12Interface {
		ID3D12Device* Device = nullptr;
		ID3D12DescriptorHeap* DescriptorHeapBackBuffers = nullptr;
		ID3D12DescriptorHeap* DescriptorHeapImGuiRender = nullptr;
		ID3D12GraphicsCommandList* CommandList = nullptr;
		ID3D12CommandQueue* CommandQueue = nullptr;
		uint64_t BuffersCounts = 0;
		_FrameContext* FrameContext = nullptr;

#ifdef IMGUI_BLUR_TEST
		ID3D12DescriptorHeap* BlurDescriptor = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE		BlurCpu = {};
		D3D12_GPU_DESCRIPTOR_HANDLE		BlurGpu = {};
		UINT							BlurHeapIncrement = 0;

		void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
		{
			static int idx = 0;
			out_cpu_desc_handle->ptr = BlurCpu.ptr + (idx * BlurHeapIncrement);
			out_gpu_desc_handle->ptr = BlurGpu.ptr + (idx * BlurHeapIncrement);
			std::cout << out_cpu_desc_handle->ptr << " - " << out_gpu_desc_handle->ptr << std::endl;
			idx += 1;
		}

		DescriptorHandle AllocateSRV(ID3D12Device* device, ID3D12Resource* resource, DXGI_FORMAT format)
		{
			DescriptorHandle handle;
			Alloc(&handle.cpu, &handle.gpu);

			D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
			desc.Format = format;
			desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			desc.Texture2D.MipLevels = 1;
			desc.Texture2D.MostDetailedMip = 0;
			device->CreateShaderResourceView(resource, &desc, handle.cpu);
			return handle;
		}

		DescriptorHandle AllocateUAV(ID3D12Device* device, ID3D12Resource* resource, DXGI_FORMAT format)
		{
			DescriptorHandle handle;
			Alloc(&handle.cpu, &handle.gpu);

			D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
			desc.Format = format;
			desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

			device->CreateUnorderedAccessView(resource, nullptr, &desc, handle.cpu);
			return handle;
		}

		void CreateBackBufferSRV(
			ID3D12Device* device,
			ID3D12Resource* backBuffer,
			D3D12_CPU_DESCRIPTOR_HANDLE* outCpuHandle,
			D3D12_GPU_DESCRIPTOR_HANDLE* outGpuHandle
		)
		{
			// Allocate descriptor
			Alloc(outCpuHandle, outGpuHandle);

			// Describe the SRV
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = backBuffer->GetDesc().Format;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;

			// Create the SRV
			device->CreateShaderResourceView(backBuffer, &srvDesc, *outCpuHandle);
		}
#endif
	}

	bool InitWindow() {

		WindowClass.cbSize = sizeof(WNDCLASSEX);
		WindowClass.style = CS_HREDRAW | CS_VREDRAW;
		WindowClass.lpfnWndProc = DefWindowProc;
		WindowClass.cbClsExtra = 0;
		WindowClass.cbWndExtra = 0;
		WindowClass.hInstance = GetModuleHandle(NULL);
		WindowClass.hIcon = NULL;
		WindowClass.hCursor = NULL;
		WindowClass.hbrBackground = NULL;
		WindowClass.lpszMenuName = NULL;
		WindowClass.lpszClassName = "MJ";
		WindowClass.hIconSm = NULL;
		RegisterClassEx(&WindowClass);
		WindowHwnd = CreateWindow(WindowClass.lpszClassName, "DirectX Window", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, WindowClass.hInstance, NULL);
		if (WindowHwnd == NULL) {
			return false;
		}
		return true;
	}

	LRESULT CALLBACK CustomWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		if (Menu::ShowMenu)
			return true;
		return CallWindowProc(Process::WndProc, hWnd, uMsg, wParam, lParam);
	}

	bool DX12::DeleteWindow() {
		DestroyWindow(WindowHwnd);
		UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance);
		if (WindowHwnd != NULL) {
			return false;
		}
		return true;
	}

	bool DX12::CreateHook(uint16_t Index, void** Original, void* Function) {
		assert(_index >= 0 && _original != NULL && _function != NULL);
		void* target = (void*)MethodsTable[Index];
		if (MH_CreateHook(target, Function, Original) != MH_OK || MH_EnableHook(target) != MH_OK) {
			return false;
		}
		return true;
	}

	void DisableHook(uint16_t Index) {
		assert(Index >= 0);
		MH_DisableHook((void*)MethodsTable[Index]);
	}

	void DisableAll() {
		MH_DisableHook(MH_ALL_HOOKS);
		free(MethodsTable);
		MethodsTable = NULL;
	}

	bool Init() {
#ifdef IS_DEBUG_VERSION
		//Sleep(1000);
		std::cout << "[DX12] InitWindow\n";
#endif
		if (InitWindow() == false) {
			return false;
		}

#ifdef IS_DEBUG_VERSION
		//Sleep(1000);
		std::cout << "[DX12] Getting modules.\n";
#endif
		HMODULE D3D12Module = GetModuleHandle("d3d12.dll");
		HMODULE DXGIModule = GetModuleHandle("dxgi.dll");
		if (D3D12Module == NULL || DXGIModule == NULL) {
			DeleteWindow();
			return false;
		}
#ifdef IS_DEBUG_VERSION
		//Sleep(1000);
		std::cout << "[DX12] Getting DXGIFactoryAddress.\n";
#endif
		void* CreateDXGIFactory = GetProcAddress(DXGIModule, "CreateDXGIFactory");
		if (CreateDXGIFactory == NULL) {
			DeleteWindow();
			return false;
		}
#ifdef IS_DEBUG_VERSION
		//Sleep(1000);
		std::cout << "[DX12] Making Factory.\n";
#endif
		IDXGIFactory* Factory;
		if (((long(__stdcall*)(const IID&, void**))(CreateDXGIFactory))(__uuidof(IDXGIFactory), (void**)&Factory) < 0) {
			DeleteWindow();
			return false;
		}
#ifdef IS_DEBUG_VERSION
		//Sleep(1000);
		std::cout << "[DX12] Getting Adapter.\n";
#endif
		IDXGIAdapter* Adapter;
		if (Factory->EnumAdapters(0, &Adapter) == DXGI_ERROR_NOT_FOUND) {
			DeleteWindow();
			return false;
		}
#ifdef IS_DEBUG_VERSION
		//Sleep(1000);
		std::cout << "[DX12] Getting CreateDevice.\n";
#endif
		void* D3D12CreateDevice = GetProcAddress(D3D12Module, "D3D12CreateDevice");
		if (D3D12CreateDevice == NULL) {
			DeleteWindow();
			return false;
		}
#ifdef IS_DEBUG_VERSION
		//Sleep(1000);
		std::cout << "[DX12] Creating device.\n";
#endif
		ID3D12Device* Device;
		if (((long(__stdcall*)(IUnknown*, D3D_FEATURE_LEVEL, const IID&, void**))(D3D12CreateDevice))(Adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&Device) < 0) {
			DeleteWindow();
			return false;
		}
#ifdef IS_DEBUG_VERSION
		//Sleep(1000);
		std::cout << "[DX12] Creating command queue.\n";
#endif
		D3D12_COMMAND_QUEUE_DESC QueueDesc;
		QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		QueueDesc.Priority = 0;
		QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		QueueDesc.NodeMask = 0;

		ID3D12CommandQueue* CommandQueue;
		if (Device->CreateCommandQueue(&QueueDesc, __uuidof(ID3D12CommandQueue), (void**)&CommandQueue) < 0) {
			DeleteWindow();
			return false;
		}
#ifdef IS_DEBUG_VERSION
		//Sleep(1000);
		std::cout << "[DX12] Creating command allocator.\n";
#endif
		ID3D12CommandAllocator* CommandAllocator;
		if (Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&CommandAllocator) < 0) {
			DeleteWindow();
			return false;
		}
#ifdef IS_DEBUG_VERSION
		//Sleep(1000);
		std::cout << "[DX12] Creating command list.\n";
#endif
		ID3D12GraphicsCommandList* CommandList;
		if (Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&CommandList) < 0) {
			DeleteWindow();
			return false;
		}

		DXGI_RATIONAL RefreshRate;
		RefreshRate.Numerator = 60;
		RefreshRate.Denominator = 1;

		DXGI_MODE_DESC BufferDesc;
		BufferDesc.Width = 100;
		BufferDesc.Height = 100;
		BufferDesc.RefreshRate = RefreshRate;
		BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		DXGI_SAMPLE_DESC SampleDesc;
		SampleDesc.Count = 1;
		SampleDesc.Quality = 0;

		DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
		SwapChainDesc.BufferDesc = BufferDesc;
		SwapChainDesc.SampleDesc = SampleDesc;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferCount = 2;
		SwapChainDesc.OutputWindow = WindowHwnd;
		SwapChainDesc.Windowed = 1;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#ifdef IS_DEBUG_VERSION
		//Sleep(1000);
		std::cout << "[DX12] Creating swapchain.\n";
#endif
		IDXGISwapChain* SwapChain;
		if (Factory->CreateSwapChain(CommandQueue, &SwapChainDesc, &SwapChain) < 0) {
			DeleteWindow();
			return false;
		}
#ifdef IS_DEBUG_VERSION
		//Sleep(1000);
		std::cout << "[DX12] Copying vtable functions.\n";
#endif
		MethodsTable = (uint64_t*)::calloc(170, sizeof(uint64_t));
		memcpy(MethodsTable, *(uint64_t**)Device, 44 * sizeof(uint64_t));
		//memcpy(MethodsTable + 44, *(uint64_t**)CommandQueue, 19 * sizeof(uint64_t));
		memcpy(MethodsTable + 44 + 19, *(uint64_t**)CommandAllocator, 9 * sizeof(uint64_t));
		memcpy(MethodsTable + 44 + 19 + 9, *(uint64_t**)CommandList, 60 * sizeof(uint64_t));
		memcpy(MethodsTable + 44 + 19 + 9 + 60, *(uint64_t**)SwapChain, 18 * sizeof(uint64_t));
		memcpy(MethodsTable + 150, *(uint64_t**)Factory, 20 * sizeof(uint64_t));

		MH_Initialize();
		Device->Release();
		Device = NULL;
		CommandQueue->Release();
		CommandQueue = NULL;
		CommandAllocator->Release();
		CommandAllocator = NULL;
		CommandList->Release();
		CommandList = NULL;
		SwapChain->Release();
		SwapChain = NULL;
		DeleteWindow();
		//#ifdef IS_DEBUG_VERSION
				//Sleep(1000);
		std::cout << "[DX12] Finished init.\n";
		//#endif
		return true;
	}

	LRESULT APIENTRY WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		//if (Menu::ShowMenu) {// && !Menu::IsCurrentlyBindingKey()) {
		ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
		//	return true;
		//}
		return CallWindowProc(Process::WndProc, hwnd, uMsg, wParam, lParam);
	}

	HRESULT APIENTRY hkPresent(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags) {
		static std::chrono::steady_clock::time_point FirstHookTime;

		ID3D12CommandQueue* queue = nullptr;
		if (g_SwapChainToQueue.size() == 0)
		{
			return oPresent(pSwapChain, SyncInterval, Flags);
		}
		else
		{
			if (g_SwapChainToQueue.contains(pSwapChain))
			{
				queue = g_SwapChainToQueue[pSwapChain];
			}
		}

		if (!isOverlayReady)
			return oPresent(pSwapChain, SyncInterval, Flags);

		if (!ImGui_Initialised) {
			if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D12Device), (void**)&DirectX12Interface::Device))) {
				ImGui::CreateContext();

				ImGuiIO& io = ImGui::GetIO(); (void)io;
				ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
				io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

				DXGI_SWAP_CHAIN_DESC Desc;
				pSwapChain->GetDesc(&Desc);
				Desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
				Desc.OutputWindow = Process::Hwnd;
				Desc.Windowed = ((GetWindowLongPtr(Process::Hwnd, GWL_STYLE) & WS_POPUP) != 0) ? false : true;

				DirectX12Interface::BuffersCounts = Desc.BufferCount;
				DirectX12Interface::FrameContext = new DirectX12Interface::_FrameContext[DirectX12Interface::BuffersCounts];

				D3D12_DESCRIPTOR_HEAP_DESC DescriptorImGuiRender = {};
				DescriptorImGuiRender.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				DescriptorImGuiRender.NumDescriptors = DirectX12Interface::BuffersCounts;
				DescriptorImGuiRender.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

				if (DirectX12Interface::Device->CreateDescriptorHeap(&DescriptorImGuiRender, IID_PPV_ARGS(&DirectX12Interface::DescriptorHeapImGuiRender)) != S_OK)
					return oPresent(pSwapChain, SyncInterval, Flags);

				ID3D12CommandAllocator* Allocator;
				if (DirectX12Interface::Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&Allocator)) != S_OK)
					return oPresent(pSwapChain, SyncInterval, Flags);

				for (size_t i = 0; i < DirectX12Interface::BuffersCounts; i++) {
					DirectX12Interface::FrameContext[i].CommandAllocator = Allocator;
				}

				if (DirectX12Interface::Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, Allocator, NULL, IID_PPV_ARGS(&DirectX12Interface::CommandList)) != S_OK ||
					DirectX12Interface::CommandList->Close() != S_OK)
					return oPresent(pSwapChain, SyncInterval, Flags);

				D3D12_DESCRIPTOR_HEAP_DESC DescriptorBackBuffers;
				DescriptorBackBuffers.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
				DescriptorBackBuffers.NumDescriptors = DirectX12Interface::BuffersCounts;
				DescriptorBackBuffers.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				DescriptorBackBuffers.NodeMask = 1;

				if (DirectX12Interface::Device->CreateDescriptorHeap(&DescriptorBackBuffers, IID_PPV_ARGS(&DirectX12Interface::DescriptorHeapBackBuffers)) != S_OK)
					return oPresent(pSwapChain, SyncInterval, Flags);

				const auto RTVDescriptorSize = DirectX12Interface::Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
				D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = DirectX12Interface::DescriptorHeapBackBuffers->GetCPUDescriptorHandleForHeapStart();


				for (size_t i = 0; i < DirectX12Interface::BuffersCounts; i++) {
					ID3D12Resource* pBackBuffer = nullptr;
					DirectX12Interface::FrameContext[i].DescriptorHandle = RTVHandle;
					pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
					DirectX12Interface::Device->CreateRenderTargetView(pBackBuffer, nullptr, RTVHandle);
					DirectX12Interface::FrameContext[i].Resource = pBackBuffer;
					RTVHandle.ptr += RTVDescriptorSize;
				}
				ImGui_ImplWin32_Init(Process::Hwnd);
				ImGui_ImplDX12_Init(DirectX12Interface::Device, DirectX12Interface::BuffersCounts, DXGI_FORMAT_R8G8B8A8_UNORM, DirectX12Interface::DescriptorHeapImGuiRender, DirectX12Interface::DescriptorHeapImGuiRender->GetCPUDescriptorHandleForHeapStart(), DirectX12Interface::DescriptorHeapImGuiRender->GetGPUDescriptorHandleForHeapStart());

				ImFontConfig config;
				ImFontConfig config2;
				config.MergeMode = false;
				config.OversampleH = 1;
				config.OversampleV = 1;
				config.PixelSnapH = true;

				config.FontDataOwnedByAtlas = false;
				config2.FontDataOwnedByAtlas = false;

				io.Fonts->AddFontFromMemoryTTF(&NunitoMedium, sizeof NunitoMedium, 18, &config, io.Fonts->GetGlyphRangesDefault());
				medium_font = io.Fonts->AddFontFromMemoryTTF(&NunitoMedium, sizeof NunitoMedium, 24, &config, io.Fonts->GetGlyphRangesDefault());
				small_font = io.Fonts->AddFontFromMemoryTTF(&NunitoMedium, sizeof NunitoMedium, 17, &config, io.Fonts->GetGlyphRangesDefault());
				icon_font = io.Fonts->AddFontFromMemoryTTF(&icomoon, sizeof icomoon, 18, &config, io.Fonts->GetGlyphRangesDefault());
				icon_big_font = io.Fonts->AddFontFromMemoryTTF(&icomoon, sizeof icomoon, 23, &config, io.Fonts->GetGlyphRangesDefault());
				logo_font = io.Fonts->AddFontFromMemoryTTF(&NunitoMedium, sizeof NunitoMedium, 25, &config2, io.Fonts->GetGlyphRangesDefault());
				small_icon_font = io.Fonts->AddFontFromMemoryTTF(&NunitoMedium, sizeof NunitoMedium, 15, &config, io.Fonts->GetGlyphRangesDefault());
				arrow_icons = io.Fonts->AddFontFromMemoryTTF(&arrowicon, sizeof arrowicon, 18, &config, io.Fonts->GetGlyphRangesDefault());
				io.Fonts->Build();
				ImGui::StyleColorsDark();

				ImGuiStyle& s = ImGui::GetStyle();
				s.FramePadding = ImVec2(10, 10);
				s.ItemSpacing = ImVec2(10, 5);
				s.FrameRounding = 5.f;
				s.WindowRounding = 15.f;
				s.WindowBorderSize = 0.f;
				s.PopupBorderSize = 0.f;
				s.WindowPadding = ImVec2(0, 0);
				s.ChildBorderSize = 0.0001f;
				s.Colors[ImGuiCol_WindowBg] = winbg_color;
				s.Colors[ImGuiCol_Border] = ImVec4(0.f, 0.f, 0.f, 0.f);
				//s.Window = 0;
				s.PopupRounding = 5.f;
				s.PopupBorderSize = 1.3f;
				s.ScrollbarSize = 4.f;
				s.ScrollbarRounding = 10.f;

				ImGui_ImplDX12_CreateDeviceObjects();
				if (!Process::WndProc)
				{
					Process::WndProc = (WNDPROC)GetWindowLongPtr(Process::Hwnd, GWLP_WNDPROC);
					SetWindowLongPtr(Process::Hwnd, GWLP_WNDPROC, (LONG_PTR)CustomWndProc);
				}
			}
			//Menu::Init();
			ImGui_Initialised = true;
			FirstHookTime = std::chrono::steady_clock::now();

		}

		static bool RenderReady = false;
		if (!RenderReady)
		{
			auto Now = std::chrono::steady_clock::now();

			if (std::chrono::duration<double>(Now - FirstHookTime).count() < 5.0f)
				return oPresent(pSwapChain, SyncInterval, Flags);
			else
			{
#ifdef IMGUI_BLUR_TEST
				DXGI_SWAP_CHAIN_DESC desc = {};
				pSwapChain->GetDesc(&desc);
				UINT bufferWidth = desc.BufferDesc.Width;
				UINT bufferHeight = desc.BufferDesc.Height;

				D3D12_DESCRIPTOR_HEAP_DESC blurHeapDesc = {};
				blurHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				blurHeapDesc.NumDescriptors = 8;
				blurHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
				DirectX12Interface::Device->CreateDescriptorHeap(&blurHeapDesc, IID_PPV_ARGS(&DirectX12Interface::BlurDescriptor));
				ImGui::BLUR.BlurDescriptor = DirectX12Interface::BlurDescriptor;

				DirectX12Interface::BlurHeapIncrement = DirectX12Interface::Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				DirectX12Interface::BlurCpu = DirectX12Interface::BlurDescriptor->GetCPUDescriptorHandleForHeapStart();
				DirectX12Interface::BlurGpu = DirectX12Interface::BlurDescriptor->GetGPUDescriptorHandleForHeapStart();

				ImGui::InitializeBlurResources(DirectX12Interface::Device, bufferWidth, bufferHeight, DXGI_FORMAT_R8G8B8A8_UNORM);

				ImGui::BLUR.srvTempA = DirectX12Interface::AllocateSRV(DirectX12Interface::Device, ImGui::BLUR.tempA, DXGI_FORMAT_R16G16B16A16_FLOAT).gpu;
				ImGui::BLUR.srvTempB = DirectX12Interface::AllocateSRV(DirectX12Interface::Device, ImGui::BLUR.tempB, DXGI_FORMAT_R16G16B16A16_FLOAT).gpu;
				ImGui::BLUR.srvTempOriginal = DirectX12Interface::AllocateSRV(DirectX12Interface::Device, ImGui::BLUR.tempOriginal, DXGI_FORMAT_R10G10B10A2_UNORM).gpu;
				ImGui::BLUR.srvConvertTemp = DirectX12Interface::AllocateSRV(DirectX12Interface::Device, ImGui::BLUR.convertTemp, DXGI_FORMAT_R16G16B16A16_FLOAT).gpu;
				ImGui::BLUR.uavTempA = DirectX12Interface::AllocateUAV(DirectX12Interface::Device, ImGui::BLUR.tempA, DXGI_FORMAT_R16G16B16A16_FLOAT).gpu;
				ImGui::BLUR.uavTempB = DirectX12Interface::AllocateUAV(DirectX12Interface::Device, ImGui::BLUR.tempB, DXGI_FORMAT_R16G16B16A16_FLOAT).gpu;
				ImGui::BLUR.uavConvertTemp = DirectX12Interface::AllocateUAV(DirectX12Interface::Device, ImGui::BLUR.convertTemp, DXGI_FORMAT_R16G16B16A16_FLOAT).gpu;
#endif

				DXGI_SWAP_CHAIN_DESC desc = {};
				pSwapChain->GetDesc(&desc);
				Cheat::ScreenMiddle.X = desc.BufferDesc.Width / 2.0;
				Cheat::ScreenMiddle.Y = desc.BufferDesc.Height / 2.0;

				RenderReady = true;
			}
		}
		DirectX12Interface::CommandQueue = queue;


		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::IsKeyPressed(ImGuiKey(CFG::cfg_ShowMenuKey)))
		{
			Menu::ShowMenu = !Menu::ShowMenu;
			io.WantCaptureKeyboard = Menu::ShowMenu;
			if (!Menu::ShowMenu)
				CFG::SaveCFG();
		}

		io.MouseDrawCursor = Menu::ShowMenu;

		DirectX12Interface::_FrameContext& CurrentFrameContext = DirectX12Interface::FrameContext[pSwapChain->GetCurrentBackBufferIndex()];
		CurrentFrameContext.CommandAllocator->Reset();
		DirectX12Interface::CommandList->Reset(CurrentFrameContext.CommandAllocator, nullptr);

#ifdef IMGUI_BLUR_TEST
		ID3D12Resource* backBuffer = nullptr;
		int Index = pSwapChain->GetCurrentBackBufferIndex();
		HRESULT Res;
		if (Index != 0)
			Res = pSwapChain->GetBuffer(Index - 1, IID_PPV_ARGS(&backBuffer));
		else
			Res = pSwapChain->GetBuffer(2, IID_PPV_ARGS(&backBuffer));

		if (SUCCEEDED(Res))
		{
			DXGI_SWAP_CHAIN_DESC desc = {};
			pSwapChain->GetDesc(&desc);
			UINT bufferWidth = desc.BufferDesc.Width;
			UINT bufferHeight = desc.BufferDesc.Height;

			ImGui::ApplyBlurToBackBuffer(DirectX12Interface::CommandList, backBuffer, bufferWidth, bufferHeight);
			DirectX12Interface::CommandList->SetDescriptorHeaps(1, &DirectX12Interface::DescriptorHeapImGuiRender);
#endif
			/* OVERLAY FOR ESP AND OTHER DRAWING */
			Render::RenderOverlay();

			/* CHEAT MENU */
			Menu::RenderMenu();
#ifdef IMGUI_BLUR_TEST
		}
#endif
		ImGui::EndFrame();

		D3D12_RESOURCE_BARRIER Barrier;
		Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		Barrier.Transition.pResource = CurrentFrameContext.Resource;
		Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		DirectX12Interface::CommandList->ResourceBarrier(1, &Barrier);
		DirectX12Interface::CommandList->OMSetRenderTargets(1, &CurrentFrameContext.DescriptorHandle, FALSE, nullptr);
		DirectX12Interface::CommandList->SetDescriptorHeaps(1, &DirectX12Interface::DescriptorHeapImGuiRender);
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DirectX12Interface::CommandList);
		Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		DirectX12Interface::CommandList->ResourceBarrier(1, &Barrier);
		DirectX12Interface::CommandList->Close();
		DirectX12Interface::CommandQueue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&DirectX12Interface::CommandList));
		return oPresent(pSwapChain, SyncInterval, Flags);
	}

	HRESULT APIENTRY hkCreateCommandQueue(
		ID3D12Device* pDevice,
		const D3D12_COMMAND_QUEUE_DESC* pDesc,
		REFIID riid,
		void** ppCommandQueue)
	{
		HRESULT hr = DX12::oCreateCommandQueue(pDevice, pDesc, riid, ppCommandQueue);

		if (SUCCEEDED(hr) && ppCommandQueue && *ppCommandQueue) {
			ID3D12CommandQueue* pQueue = static_cast<ID3D12CommandQueue*>(*ppCommandQueue);
			// Store this queue somewhere (e.g., global or map by thread/device/etc.)
			DirectX12Interface::CommandQueue = pQueue;
		}

		return hr;
	}

	HRESULT APIENTRY hkCreateSwapChain(
		IDXGIFactory* pFactory,
		IUnknown* pDeviceOrQueue,
		DXGI_SWAP_CHAIN_DESC* pDesc,
		IDXGISwapChain** ppSwapChain)
	{
		HRESULT hr = DX12::oCreateSwapChain(pFactory, pDeviceOrQueue, pDesc, ppSwapChain);

		if (SUCCEEDED(hr) && ppSwapChain && *ppSwapChain) {
			//std::cout << "SWAPCHAIN MADE\n";

			IDXGISwapChain* swap = *ppSwapChain;
			g_SwapChainToQueue[*ppSwapChain] = static_cast<ID3D12CommandQueue*>(pDeviceOrQueue);
		}

		return hr;
	}

	HRESULT WINAPI hkD3D12CreateDevice(
		IUnknown* pAdapter,
		D3D_FEATURE_LEVEL MinimumFeatureLevel,
		REFIID riid,
		void** ppDevice)
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}
		return oD3D12CreateDevice(pAdapter, MinimumFeatureLevel, riid, ppDevice);
	}

}