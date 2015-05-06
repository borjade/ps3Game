#include <stdio.h>
#include <stdlib.h>	

#include <string>
#include <iostream>

#include <sys/process.h>

#include "PhysicsSystem.h"
#include "Game.h"
#include "Renderer.h"
#include "Input.h"
#include "timer.h"
#include "camera.h"

//<----- ZE THREADING INCLUDES AND FORWARD DECLARATIONS BEGIN ----->
//Multithreading attempt number 2: Begin includes
#include <spu_printf.h> //And access to the SPU printf functions
#include <sys/spu_initialize.h> //and initialise SPUs
#include <sys/spu_utility.h> //and load in SPU images
#include <sys/paths.h> //and know about SYS_APP_HOME
#include <sys/ppu_thread.h> //and make PPU threads
#include <sys/spu_thread.h> //and make SPU threads
#include <sys/event.h> //and process events

void ppu_render_thread_entry(uint64_t arg);
static sys_event_queue_t print_queue;
bool stopRender = false;

void ppu_physics_thread_entry(uint64_t phys_arg);
static sys_event_queue_t physics_queue;
bool stopPhysics = false;

//SPU Thread specific includes and defines
//#include "matrixshared.h"

#define MATRIX_DIMENSION 4
#define STACK_SIZE 4096
#define PRIO 200
#define MAX_PHYSICAL_SPU 1
#define MAX_RAW_SPU 0
#define SPU_SELF "/MatrixSPU.self"
#define SPU_PROG (SYS_APP_HOME SPU_SELF)

void print_matrix(float* mat, unsigned int dimension);
//<----- ZE THREADING INCLUDES AND FORWARD DECLARATIONS END ----->
//float Tester[16];
Matrix4 Tester;


SYS_PROCESS_PARAM(1001, 0x10000)

bool done = false;
Game* game;
Timer gameTimer;

//Camera* camera;

void start_button()		{
	done = true;
	stopRender = true;
	stopPhysics = true;
	std::cout << "Pressed start button!" << std::endl;
}

//void select_button()		{
//	camera->SetPosition(Vector3(0,0,10));
//	camera->SetPitch(0.0f);
//	camera->SetYaw(0.0f);
//}


int Quit()
{
	PhysicsSystem::Destroy();
	Input::Destroy();
	Renderer::Destroy();
	return 0;
}


int main(void)	{

	//std::cout << "All-New intro to GCM V1.0!\n" << std::endl;

	/*---------------Game/Renderer/Physics/Input Initialisation-----------------*/
	Input::Initialise();
	PhysicsSystem::Initialise();
	Renderer::Initialise();
	game = new Game();
	/*--------------------------------------------------------------------------*/

	
	//If the start button is pressed, call this function!
	Input::SetPadFunction(INPUT_START, start_button);
	//Input::SetPadFunction(INPUT_SELECT,	select_button);
	int return_val;
	//<----- ZE THREAD CONSTRUCTION BEGINS HERE ----->
	//<---Physics Thread--->
	sys_ppu_thread_t physicsThread;
	uint64_t physics_thread_arg = 100;
	uint64_t physics_priority = 600;
	size_t physics_stack_size = 0xfffff;
	char* physics_thread_name = "Physics Thread";
	sys_spu_thread_group_t physics_thread_group;
	sys_event_queue_attribute_t physics_queue_attr;

	sys_event_queue_attribute_initialize(physics_queue_attr);

	return_val = sys_event_queue_create(&physics_queue, &physics_queue_attr, 0x02, 127);
	if (return_val != CELL_OK)
	{
		printf("PPU: physics sys_event_queue_create failed %i\n", return_val);
		exit(return_val);
	}

	return_val = sys_ppu_thread_create(&physicsThread, ppu_physics_thread_entry, 0, physics_priority, physics_stack_size, SYS_PPU_THREAD_CREATE_JOINABLE, physics_thread_name);
	if (return_val != CELL_OK)
	{
		printf("PPU: PPU physics thread creation failed %i\n", return_val);
		exit(return_val);
	}
	//<----- ZE THREAD CONSTRUCTION ENDS HERE ----->

	////<----- SPU THREAD CONSTRUCTION ----->
	//sys_spu_thread_t thread_spu;
	//sys_spu_thread_group_t thread_group;
	//sys_event_queue_t signal_queue;
	//sys_event_queue_attribute_t queue_attr;
	//sys_event_t event;
	//sys_spu_image_t spu_img;

	//return_val = sys_spu_initialize(MAX_PHYSICAL_SPU, MAX_RAW_SPU);
	//if (return_val != CELL_OK)
	//{
	//	printf("PPU: Couldn't initialise the SPU's! %i\n", return_val);
	//	exit(return_val);
	//}

	//sys_spu_thread_group_attribute_t group_attr;
	//group_attr.type = SYS_SPU_THREAD_GROUP_TYPE_NORMAL;

	//return_val = sys_spu_thread_group_create(&thread_group, MAX_PHYSICAL_SPU, 100, &group_attr);
	//if (return_val != CELL_OK)
	//{
	//	printf("PPU: SPU thread group create failed %i\n", return_val);
	//	exit(return_val);
	//}

	//return_val = sys_spu_image_open(&spu_img, SPU_PROG);
	//if (return_val != CELL_OK)
	//{
	//	printf("PPU: sys_spu_image_open failed %i\n", return_val);
	//	exit(return_val);
	//}

	//sys_spu_thread_attribute_t thread_attr;
	//sys_spu_thread_argument_t thread_args;

	/*return_val = sys_spu_thread_initialize(&thread_spu, thread_group, 0, &spu_img, &thread_attr, &thread_args);
	if (return_val != CELL_OK)
	{
		printf("PPU: SPU thread init failed %i\n", return_val);
		exit(return_val);
	}*/

	//sys_event_queue_attribute_initialize(queue_attr);

	//return_val = sys_event_queue_create(&signal_queue, &queue_attr, SYS_EVENT_PORT_LOCAL, 127);
	//if (return_val != CELL_OK)
	//{
	//	printf("sys_event_queue_create failed %i\n", return_val);
	//	exit(return_val);
	//}

	////connecting the queue to the SPU thread
	//return_val = sys_spu_thread_connect_event(thread_spu, signal_queue, SYS_SPU_THREAD_EVENT_USER, SPU_SIGNAL_PORT);
	//if (return_val != CELL_OK)
	//{
	//	printf("PPU: SPU Queue connect failed %i\n", return_val);
	//	exit(return_val);
	//}

	//return_val = sys_spu_thread_group_start(thread_group);
	//if (return_val != CELL_OK)
	//{
	//	printf("PPU: SPU thread group start failed %i\n", return_val);
	//	exit(return_val);
	//}

	//spu_data_t* data = (spu_data_t*)memalign(DATA_ALIGN, sizeof(spu_data_t)); //Instance of our SPU data structure

	//data->dimension = MATRIX_DIMENSION;
	//unsigned int data_size = data->dimension * data->dimension * sizeof(float);
	//unsigned int mat_data_size = data->dimension * data->dimension * sizeof(Matrix4);

	//data->matrixA = (float*)memalign(DATA_ALIGN, data_size); //byte aligning our matrices
	//data->matrixB = (float*)memalign(DATA_ALIGN, data_size);
	//data->matrixC = (float*)memalign(DATA_ALIGN, data_size);
	////<----- END SPU THREAD CONSTRUCTION ----->
	////Tester = (float*)memalign(DATA_ALIGN, data_size);
	//data->viewMat = (Matrix4*)memalign(DATA_ALIGN, mat_data_size);
	//data->viewMat = &Renderer::GetRenderer().GetViewMatrix();

	//data->projMat = (Matrix4*)memalign(DATA_ALIGN, mat_data_size);
	//data->projMat = &Renderer::GetRenderer().GetProjectionMatrix();

	//data->modelMat = (Matrix4*)memalign(DATA_ALIGN, mat_data_size);
	//data->modelMat = &Renderer::GetRenderer().GetModelMatrix();

	while (!done)
	{
		float msec = gameTimer.GetTimedMS();
		//Input::UpdateJoypad();
		//game->UpdatePhysics(msec);
		//game->UpdateGame(msec);
		
		game->UpdateGame(msec);
		game->UpdateRendering(msec);

		////<-----Model View Projection matrix calculations----->
		////matrix_to_spu_matrix(data->matrixA, data->dimension, Renderer::GetRenderer().GetProjectionMatrix());
		//matrix_to_spu_matrix(data->matrixA, data->dimension, &Renderer::GetRenderer().GetProjectionMatrix());
		//matrix_to_spu_matrix(data->matrixB, data->dimension, &Renderer::GetRenderer().GetViewMatrix());
		////matrix_to_spu_matrix(data->matrixB, data->dimension, &Renderer::GetRenderer().GetModelMatrix());
		//matrix_to_identity(data->matrixC, data->dimension);

		//sys_spu_thread_write_snr(thread_spu, 0, (std::uint32_t)data); //(which spu recieves the signal, which of the two SNRs will the signal be placed in, what is placed in the SNR)

		//sys_event_queue_receive(signal_queue, &event, SYS_NO_TIMEOUT); //blocking function to to stop the PPU running code until an event is recieved
		//
		////print_matrix(data->matrixC, data->dimension);
		//spu_matrix_equaliser(data->matrixA, data->matrixC, data->dimension);
		////print_matrix(data->matrixA, data->dimension);
		//matrix_to_identity(data->matrixC, data->dimension);
		//matrix_to_spu_matrix(data->matrixB, data->dimension, &Renderer::GetRenderer().GetModelMatrix());

		//sys_spu_thread_write_snr(thread_spu, 0, (std::uint32_t)data); //(which spu recieves the signal, which of the two SNRs will the signal be placed in, what is placed in the SNR)

		//sys_event_queue_receive(signal_queue, &event, SYS_NO_TIMEOUT); //blocking function to to stop the PPU running code until an event is recieved
		////<-----Model View Projection matrix calculations end----->
		////print_matrix(data->matrixC, data->dimension);
		//spu_matrix_to_ppu_matrix(data->matrixC, data->dimension, Renderer::GetRenderer().GetMVPMatrix()); //BUG IN WRITING TO THE MATRIX HERE!!!!
		//matrix_to_identity(data->matrixC, data->dimension);
		
		
		//game->UpdatePhysics(msec);
		////Input::UpdateJoypad();	//Receive latest joypad input for all joypads
		////renderer.RenderScene();	//Render the scene
	}
	//If we get here, joypad A has had its start button pressed

	//Tester = Renderer::GetRenderer().GetMVPMatrix();

	//<----- LET THE THREADING HUNGER GAMES BEGIN ----->

	////<---SPU THREAD DESTRUCTON--->
	//sys_spu_thread_write_snr(thread_spu, 0, 0);
	//sys_spu_thread_group_join(thread_group, 0, 0);
	//sys_spu_thread_group_destroy(thread_group);
	//sys_event_queue_destroy(signal_queue, SYS_EVENT_QUEUE_DESTROY_FORCE);

	//return_val = sys_spu_image_close(&spu_img);
	//if (return_val != CELL_OK)
	//{
	//	printf("PPU: sys_spu_image_close failed %i\n", return_val);
	//	exit(return_val);
	//}
	////<---SPU THREAD DESTRUCTON END--->

	//We are cleaning up here by destroying our stuff so that we can exit the program cleanly
	std::cout << "Thread group joins the event queues..." << std::endl;
	sys_spu_thread_group_join(physics_thread_group, 0, 0);
	/*sys_spu_thread_group_join(render_thread_group, 0, 0);
*/
	std::cout << "Destroying the thread groups..." << std::endl;
	sys_spu_thread_group_destroy(physics_thread_group);
	//sys_spu_thread_group_destroy(render_thread_group);

	std::cout << "Destroying the event queues..." << std::endl;
	sys_event_queue_destroy(physics_queue, SYS_EVENT_QUEUE_DESTROY_FORCE);
	//sys_event_queue_destroy(print_queue, SYS_EVENT_QUEUE_DESTROY_FORCE);

//	//<----- AND NOW THE THREADS ARE DEAD ----->

	std::cout << "Quitting..." << std::endl;

	return 0;
	//delete game;
	//return Quit();
	
}


void ppu_physics_thread_entry(uint64_t phys_arg)
{
	int return_val;
	sys_event_t event;
	while (!stopRender)
	{
		float msec = gameTimer.GetTimedMS();
		Input::UpdateJoypad();
		game->UpdatePhysics(msec);
		
		 /* Right now this doesn't work bc it's all empty */
		//printf("Physics Thread Update\n");
	}


	for (;;)
	{
		return_val = sys_event_queue_receive(physics_queue, &event, SYS_NO_TIMEOUT);
		if (return_val != CELL_OK)
		{
			if (return_val == ECANCELED)
			{
				printf("PPU thread: Physics Event Queue destroyed! Exiting...\n");
			}
			else
			{
				printf("PPU thread: Physics Event Queue receive failed: %i\n", return_val);
			}
			break;
		}
	}
	sys_ppu_thread_exit(0);
}


void print_matrix(float* mat, unsigned int dimension)
{
	for (int x = 0; x < dimension; ++x)
	{
		for (int y = 0; y < dimension; ++y)
		{
			printf("%f\t", (float)mat[((x*dimension) + y)]);
		}
		printf("\n");
	}
}