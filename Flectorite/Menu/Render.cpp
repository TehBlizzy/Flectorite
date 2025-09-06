#include "Render.h"

namespace Render
{
#define PAD(base, alignment)	(((base)+(alignment)-1) & ~((alignment)-1))
#define PADP(base, alignment)	((void *) PAD((intptr_t) (base), (alignment)))

	void RenderOverlay()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
		ImGui::Begin("Overlay", nullptr,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
		auto pDrawList = ImGui::GetWindowDrawList();

		void* data = Commands.cmds;
		bool isDone = false;
		while (!isDone)
		{
			data = PADP(data, sizeof(void*));
			switch (*static_cast<const int*>(data))
			{
			case RD_TEXT:
			{
				rd_text* cmd;
				cmd = static_cast<rd_text*>(data);
				ImVec2 textSize = ImGui::CalcTextSize(cmd->txt);
				if (cmd->bg)
				{
					pDrawList->AddRectFilled(ImVec2((cmd->x - (textSize.x / 2)) - 1, cmd->y - 1),
						ImVec2((cmd->x + (textSize.x / 2)) + 1, cmd->y + textSize.y + 1),
						ImColor(0.0f, 0.0f, 0.0f, 0.35f));
				}
				pDrawList->AddText(ImVec2(cmd->x - (textSize.x / 2), cmd->y),
					ImColor(cmd->color[0], cmd->color[1], cmd->color[2], cmd->color[3]),
					(char*)cmd->txt);
				data = static_cast<void*>(cmd + 1);
				break;
			}
			case RD_RECT:
			{
				rd_rect* cmd;
				cmd = static_cast<rd_rect*>(data);
				if (cmd->x > 0 && cmd->x < io.DisplaySize.x && cmd->y > 0 && cmd->y < io.DisplaySize.y)
					if (cmd->w > 0 && cmd->w < io.DisplaySize.x && cmd->h > 0 && cmd->h < io.DisplaySize.y)
						pDrawList->AddRect(ImVec2(cmd->x, cmd->y), ImVec2(cmd->w, cmd->h), ImColor(cmd->color[0], cmd->color[1], cmd->color[2], cmd->color[3]));
				data = static_cast<void*>(cmd + 1);
				break;
			}
			case RD_FILLRECT:
			{
				rd_fillrect* cmd;
				cmd = static_cast<rd_fillrect*>(data);
				pDrawList->AddRectFilled(ImVec2(cmd->x, cmd->y), ImVec2(cmd->w, cmd->h),
					ImColor(cmd->color[0], cmd->color[1], cmd->color[2], cmd->color[3]));
				data = static_cast<void*>(cmd + 1);
				break;
			}
			case RD_LINE:
			{
				rd_line* cmd;
				cmd = static_cast<rd_line*>(data);
				pDrawList->AddLine(ImVec2(cmd->x, cmd->y), ImVec2(cmd->x2, cmd->y2),
					ImColor(cmd->color[0], cmd->color[1], cmd->color[2], cmd->color[3]), 1.0f);
				data = static_cast<void*>(cmd + 1);
				break;
			}
			case RD_CIRCLE:
			{
				rd_circle* cmd = static_cast<rd_circle*>(data);
				if (cmd->radius > 0.0f &&
					cmd->x - cmd->radius < io.DisplaySize.x &&
					cmd->x + cmd->radius > 0 &&
					cmd->y - cmd->radius < io.DisplaySize.y &&
					cmd->y + cmd->radius > 0)
				{
					pDrawList->AddCircle(
						ImVec2(cmd->x, cmd->y),
						cmd->radius,
						ImColor(cmd->color[0], cmd->color[1], cmd->color[2], cmd->color[3]),
						0,
						1.5f
					);
				}
				data = static_cast<void*>(cmd + 1);
				break;
			}
			case RD_3DBOX:
			{
				rd_3dbox* cmd = static_cast<rd_3dbox*>(data);
				SDK::FVector center(cmd->center[0], cmd->center[1], cmd->center[2]);
				SDK::FVector extent(cmd->extent[0], cmd->extent[1], cmd->extent[2]);
				ImColor color(cmd->color[0], cmd->color[1], cmd->color[2], cmd->color[3]);

				SDK::FVector corners[8] = {
					{center.X - extent.X, center.Y - extent.Y, center.Z - extent.Z},
					{center.X + extent.X, center.Y - extent.Y, center.Z - extent.Z},
					{center.X + extent.X, center.Y + extent.Y, center.Z - extent.Z},
					{center.X - extent.X, center.Y + extent.Y, center.Z - extent.Z},
					{center.X - extent.X, center.Y - extent.Y, center.Z + extent.Z},
					{center.X + extent.X, center.Y - extent.Y, center.Z + extent.Z},
					{center.X + extent.X, center.Y + extent.Y, center.Z + extent.Z},
					{center.X - extent.X, center.Y + extent.Y, center.Z + extent.Z}
				};

				SDK::FVector2D screenCorners[8];
				bool validCorners[8];
				int validCount = 0;

				for (int i = 0; i < 8; i++)
				{
					validCorners[i] = Cheat::WorldToScreen(corners[i], &screenCorners[i]);
					if (validCorners[i]) validCount++;
				}

				if (validCount > 0)
				{
					if (validCorners[0] && validCorners[1])
						pDrawList->AddLine(ImVec2(screenCorners[0].X, screenCorners[0].Y), ImVec2(screenCorners[1].X, screenCorners[1].Y), color, 1.5f);
					if (validCorners[1] && validCorners[2])
						pDrawList->AddLine(ImVec2(screenCorners[1].X, screenCorners[1].Y), ImVec2(screenCorners[2].X, screenCorners[2].Y), color, 1.5f);
					if (validCorners[2] && validCorners[3])
						pDrawList->AddLine(ImVec2(screenCorners[2].X, screenCorners[2].Y), ImVec2(screenCorners[3].X, screenCorners[3].Y), color, 1.5f);
					if (validCorners[3] && validCorners[0])
						pDrawList->AddLine(ImVec2(screenCorners[3].X, screenCorners[3].Y), ImVec2(screenCorners[0].X, screenCorners[0].Y), color, 1.5f);

					if (validCorners[4] && validCorners[5])
						pDrawList->AddLine(ImVec2(screenCorners[4].X, screenCorners[4].Y), ImVec2(screenCorners[5].X, screenCorners[5].Y), color, 1.5f);
					if (validCorners[5] && validCorners[6])
						pDrawList->AddLine(ImVec2(screenCorners[5].X, screenCorners[5].Y), ImVec2(screenCorners[6].X, screenCorners[6].Y), color, 1.5f);
					if (validCorners[6] && validCorners[7])
						pDrawList->AddLine(ImVec2(screenCorners[6].X, screenCorners[6].Y), ImVec2(screenCorners[7].X, screenCorners[7].Y), color, 1.5f);
					if (validCorners[7] && validCorners[4])
						pDrawList->AddLine(ImVec2(screenCorners[7].X, screenCorners[7].Y), ImVec2(screenCorners[4].X, screenCorners[4].Y), color, 1.5f);

					if (validCorners[0] && validCorners[4])
						pDrawList->AddLine(ImVec2(screenCorners[0].X, screenCorners[0].Y), ImVec2(screenCorners[4].X, screenCorners[4].Y), color, 1.5f);
					if (validCorners[1] && validCorners[5])
						pDrawList->AddLine(ImVec2(screenCorners[1].X, screenCorners[1].Y), ImVec2(screenCorners[5].X, screenCorners[5].Y), color, 1.5f);
					if (validCorners[2] && validCorners[6])
						pDrawList->AddLine(ImVec2(screenCorners[2].X, screenCorners[2].Y), ImVec2(screenCorners[6].X, screenCorners[6].Y), color, 1.5f);
					if (validCorners[3] && validCorners[7])
						pDrawList->AddLine(ImVec2(screenCorners[3].X, screenCorners[3].Y), ImVec2(screenCorners[7].X, screenCorners[7].Y), color, 1.5f);
				}

				data = static_cast<void*>(cmd + 1);
				break;
			}
			case RD_3DBOX_ROTATED:
			{
				rd_3dbox_rotated* cmd = static_cast<rd_3dbox_rotated*>(data);
				SDK::FVector center(cmd->center[0], cmd->center[1], cmd->center[2]);
				SDK::FVector extent(cmd->extent[0], cmd->extent[1], cmd->extent[2]);
				SDK::FRotator rotation(cmd->rotation[0], cmd->rotation[1], cmd->rotation[2]);
				ImColor color(cmd->color[0], cmd->color[1], cmd->color[2], cmd->color[3]);

				float pitch = rotation.Pitch * (3.14159f / 180.0f);
				float yaw = rotation.Yaw * (3.14159f / 180.0f);
				float roll = rotation.Roll * (3.14159f / 180.0f);

				float cosPitch = cosf(pitch), sinPitch = sinf(pitch);
				float cosYaw = cosf(yaw), sinYaw = sinf(yaw);
				float cosRoll = cosf(roll), sinRoll = sinf(roll);

				SDK::FVector localCorners[8] = {
					{-extent.X, -extent.Y, -extent.Z},
					{+extent.X, -extent.Y, -extent.Z},
					{+extent.X, +extent.Y, -extent.Z},
					{-extent.X, +extent.Y, -extent.Z},
					{-extent.X, -extent.Y, +extent.Z},
					{+extent.X, -extent.Y, +extent.Z},
					{+extent.X, +extent.Y, +extent.Z},
					{-extent.X, +extent.Y, +extent.Z}
				};

				SDK::FVector corners[8];
				for (int i = 0; i < 8; i++)
				{
					SDK::FVector local = localCorners[i];

					float x = local.X * (cosYaw * cosRoll - sinYaw * sinPitch * sinRoll) +
						local.Y * (-cosYaw * sinRoll - sinYaw * sinPitch * cosRoll) +
						local.Z * (-sinYaw * cosPitch);

					float y = local.X * (sinYaw * cosRoll + cosYaw * sinPitch * sinRoll) +
						local.Y * (-sinYaw * sinRoll + cosYaw * sinPitch * cosRoll) +
						local.Z * (cosYaw * cosPitch);

					float z = local.X * (cosPitch * sinRoll) +
						local.Y * (cosPitch * cosRoll) +
						local.Z * (-sinPitch);

					corners[i] = { center.X + x, center.Y + y, center.Z + z };
				}
				SDK::FVector2D screenCorners[8];
				bool validCorners[8];
				int validCount = 0;

				for (int i = 0; i < 8; i++)
				{
					validCorners[i] = Cheat::WorldToScreen(corners[i], &screenCorners[i]);
					if (validCorners[i]) validCount++;
				}

				if (validCount > 0)
				{
					if (validCorners[0] && validCorners[1])
						pDrawList->AddLine(ImVec2(screenCorners[0].X, screenCorners[0].Y), ImVec2(screenCorners[1].X, screenCorners[1].Y), color, 1.5f);
					if (validCorners[1] && validCorners[2])
						pDrawList->AddLine(ImVec2(screenCorners[1].X, screenCorners[1].Y), ImVec2(screenCorners[2].X, screenCorners[2].Y), color, 1.5f);
					if (validCorners[2] && validCorners[3])
						pDrawList->AddLine(ImVec2(screenCorners[2].X, screenCorners[2].Y), ImVec2(screenCorners[3].X, screenCorners[3].Y), color, 1.5f);
					if (validCorners[3] && validCorners[0])
						pDrawList->AddLine(ImVec2(screenCorners[3].X, screenCorners[3].Y), ImVec2(screenCorners[0].X, screenCorners[0].Y), color, 1.5f);

					if (validCorners[4] && validCorners[5])
						pDrawList->AddLine(ImVec2(screenCorners[4].X, screenCorners[4].Y), ImVec2(screenCorners[5].X, screenCorners[5].Y), color, 1.5f);
					if (validCorners[5] && validCorners[6])
						pDrawList->AddLine(ImVec2(screenCorners[5].X, screenCorners[5].Y), ImVec2(screenCorners[6].X, screenCorners[6].Y), color, 1.5f);
					if (validCorners[6] && validCorners[7])
						pDrawList->AddLine(ImVec2(screenCorners[6].X, screenCorners[6].Y), ImVec2(screenCorners[7].X, screenCorners[7].Y), color, 1.5f);
					if (validCorners[7] && validCorners[4])
						pDrawList->AddLine(ImVec2(screenCorners[7].X, screenCorners[7].Y), ImVec2(screenCorners[4].X, screenCorners[4].Y), color, 1.5f);

					if (validCorners[0] && validCorners[4])
						pDrawList->AddLine(ImVec2(screenCorners[0].X, screenCorners[0].Y), ImVec2(screenCorners[4].X, screenCorners[4].Y), color, 1.5f);
					if (validCorners[1] && validCorners[5])
						pDrawList->AddLine(ImVec2(screenCorners[1].X, screenCorners[1].Y), ImVec2(screenCorners[5].X, screenCorners[5].Y), color, 1.5f);
					if (validCorners[2] && validCorners[6])
						pDrawList->AddLine(ImVec2(screenCorners[2].X, screenCorners[2].Y), ImVec2(screenCorners[6].X, screenCorners[6].Y), color, 1.5f);
					if (validCorners[3] && validCorners[7])
						pDrawList->AddLine(ImVec2(screenCorners[3].X, screenCorners[3].Y), ImVec2(screenCorners[7].X, screenCorners[7].Y), color, 1.5f);
				}

				data = static_cast<void*>(cmd + 1);
				break;
			}
			case RD_3DSPHERE:
			{
				rd_3dsphere* cmd = static_cast<rd_3dsphere*>(data);
				SDK::FVector center(cmd->center[0], cmd->center[1], cmd->center[2]);
				float radius = cmd->radius;
				ImColor color(cmd->color[0], cmd->color[1], cmd->color[2], cmd->color[3]);

				const int latLines = 3;
				const int lonLines = 4;
				const int pointsPerCircle = 18;

				for (int lat = 1; lat < latLines; lat++) {
					float phi = (float)lat * 3.14159f / latLines;
					float y = radius * cosf(phi);
					float circleRadius = radius * sinf(phi);

					SDK::FVector2D prevScreen;
					bool prevValid = false;

					for (int i = 0; i <= pointsPerCircle; i++) {
						float theta = (float)i * 2.0f * 3.14159f / pointsPerCircle;
						float x = circleRadius * cosf(theta);
						float z = circleRadius * sinf(theta);

						SDK::FVector worldPos = { center.X + x, center.Y + z, center.Z + y };
						SDK::FVector2D screenPos;
						bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

						if (valid && prevValid) {
							pDrawList->AddLine(ImVec2(prevScreen.X, prevScreen.Y), ImVec2(screenPos.X, screenPos.Y), color, 1.0f);
						}

						prevScreen = screenPos;
						prevValid = valid;
					}
				}

				for (int lon = 0; lon < lonLines; lon++) {
					float theta = (float)lon * 2.0f * 3.14159f / lonLines;

					SDK::FVector2D prevScreen;
					bool prevValid = false;

					for (int i = 0; i <= pointsPerCircle; i++) {
						float phi = (float)i * 3.14159f / pointsPerCircle;
						float x = radius * sinf(phi) * cosf(theta);
						float y = radius * cosf(phi);
						float z = radius * sinf(phi) * sinf(theta);

						SDK::FVector worldPos = { center.X + x, center.Y + z, center.Z + y };
						SDK::FVector2D screenPos;
						bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

						if (valid && prevValid) {
							pDrawList->AddLine(ImVec2(prevScreen.X, prevScreen.Y), ImVec2(screenPos.X, screenPos.Y), color, 1.0f);
						}

						prevScreen = screenPos;
						prevValid = valid;
					}
				}

				data = static_cast<void*>(cmd + 1);
				break;
			}
			case RD_3DSPHERE_ROTATED:
			{
				rd_3dsphere_rotated* cmd = static_cast<rd_3dsphere_rotated*>(data);
				SDK::FVector center(cmd->center[0], cmd->center[1], cmd->center[2]);
				float radius = cmd->radius;
				SDK::FRotator rotation(cmd->rotation[0], cmd->rotation[1], cmd->rotation[2]);
				ImColor color(cmd->color[0], cmd->color[1], cmd->color[2], cmd->color[3]);

				float pitch = rotation.Pitch * (3.14159f / 180.0f);
				float yaw = rotation.Yaw * (3.14159f / 180.0f);
				float roll = rotation.Roll * (3.14159f / 180.0f);

				float cosPitch = cosf(pitch), sinPitch = sinf(pitch);
				float cosYaw = cosf(yaw), sinYaw = sinf(yaw);
				float cosRoll = cosf(roll), sinRoll = sinf(roll);

				const int latLines = 3;
				const int lonLines = 4;
				const int pointsPerCircle = 18;

				for (int lat = 1; lat < latLines; lat++) {
					float phi = (float)lat * 3.14159f / latLines;
					float y = radius * cosf(phi);
					float circleRadius = radius * sinf(phi);

					SDK::FVector2D prevScreen;
					bool prevValid = false;

					for (int i = 0; i <= pointsPerCircle; i++) {
						float theta = (float)i * 2.0f * 3.14159f / pointsPerCircle;
						float x = circleRadius * cosf(theta);
						float z = circleRadius * sinf(theta);

						float rotX = x * (cosYaw * cosRoll - sinYaw * sinPitch * sinRoll) +
							y * (-cosYaw * sinRoll - sinYaw * sinPitch * cosRoll) +
							z * (-sinYaw * cosPitch);

						float rotY = x * (sinYaw * cosRoll + cosYaw * sinPitch * sinRoll) +
							y * (-sinYaw * sinRoll + cosYaw * sinPitch * cosRoll) +
							z * (cosYaw * cosPitch);

						float rotZ = x * (cosPitch * sinRoll) +
							y * (cosPitch * cosRoll) +
							z * (-sinPitch);

						SDK::FVector worldPos = { center.X + rotX, center.Y + rotY, center.Z + rotZ };
						SDK::FVector2D screenPos;
						bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

						if (valid && prevValid) {
							pDrawList->AddLine(ImVec2(prevScreen.X, prevScreen.Y), ImVec2(screenPos.X, screenPos.Y), color, 1.0f);
						}

						prevScreen = screenPos;
						prevValid = valid;
					}
				}

				for (int lon = 0; lon < lonLines; lon++) {
					float theta = (float)lon * 2.0f * 3.14159f / lonLines;

					SDK::FVector2D prevScreen;
					bool prevValid = false;

					for (int i = 0; i <= pointsPerCircle; i++) {
						float phi = (float)i * 3.14159f / pointsPerCircle;
						float x = radius * sinf(phi) * cosf(theta);
						float y = radius * cosf(phi);
						float z = radius * sinf(phi) * sinf(theta);

						float rotX = x * (cosYaw * cosRoll - sinYaw * sinPitch * sinRoll) +
							y * (-cosYaw * sinRoll - sinYaw * sinPitch * cosRoll) +
							z * (-sinYaw * cosPitch);

						float rotY = x * (sinYaw * cosRoll + cosYaw * sinPitch * sinRoll) +
							y * (-sinYaw * sinRoll + cosYaw * sinPitch * cosRoll) +
							z * (cosYaw * cosPitch);

						float rotZ = x * (cosPitch * sinRoll) +
							y * (cosPitch * cosRoll) +
							z * (-sinPitch);

						SDK::FVector worldPos = { center.X + rotX, center.Y + rotY, center.Z + rotZ };
						SDK::FVector2D screenPos;
						bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

						if (valid && prevValid) {
							pDrawList->AddLine(ImVec2(prevScreen.X, prevScreen.Y), ImVec2(screenPos.X, screenPos.Y), color, 1.0f);
						}

						prevScreen = screenPos;
						prevValid = valid;
					}
				}

				data = static_cast<void*>(cmd + 1);
				break;
			}
			case RD_3DSPHERE_LIGHTBEAM:
			{
				rd_3dsphere_lightbeam* cmd = static_cast<rd_3dsphere_lightbeam*>(data);
				SDK::FVector center(cmd->center[0], cmd->center[1], cmd->center[2]);
				float radius = cmd->radius;
				SDK::FRotator rotation(cmd->rotation[0], cmd->rotation[1], cmd->rotation[2]);
				float beamHeight = cmd->beamHeight;
				float beamIntensity = cmd->beamIntensity;
				ImColor color(cmd->color[0], cmd->color[1], cmd->color[2], cmd->color[3]);

				float distanceFromPlayer = 0.0f;
				bool useSimpleRender = false;

				if (Cheat::LocalPlayerCharacter) {
					SDK::FVector playerPos = Cheat::LocalPlayerCharacter->K2_GetActorLocation();
					distanceFromPlayer = center.GetDistanceTo(playerPos) / 100.0f;
					useSimpleRender = distanceFromPlayer > 50.0f;
				}

				if (useSimpleRender) {
					const int simpleLatLines = 2;
					const int simplePointsPerCircle = 8;

					for (int lat = 1; lat < simpleLatLines; lat++) {
						float phi = (float)lat * 3.14159f / simpleLatLines;
						float y = radius * cosf(phi);
						float circleRadius = radius * sinf(phi);

						SDK::FVector2D prevScreen;
						bool prevValid = false;

						for (int i = 0; i <= simplePointsPerCircle; i++) {
							float theta = (float)i * 2.0f * 3.14159f / simplePointsPerCircle;
							float x = circleRadius * cosf(theta);
							float z = circleRadius * sinf(theta);

							SDK::FVector worldPos = { center.X + x, center.Y + y, center.Z + z };
							SDK::FVector2D screenPos;
							bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

							if (valid && prevValid) {
								pDrawList->AddLine(ImVec2(prevScreen.X, prevScreen.Y), ImVec2(screenPos.X, screenPos.Y), color, 1.0f);
							}

							prevScreen = screenPos;
							prevValid = valid;
						}
					}

					const int simpleLonLines = 3;
					for (int lon = 0; lon < simpleLonLines; lon++) {
						float theta = (float)lon * 2.0f * 3.14159f / simpleLonLines;

						SDK::FVector2D prevScreen;
						bool prevValid = false;

						for (int i = 0; i <= simplePointsPerCircle; i++) {
							float phi = (float)i * 3.14159f / simplePointsPerCircle;
							float x = radius * sinf(phi) * cosf(theta);
							float y = radius * cosf(phi);
							float z = radius * sinf(phi) * sinf(theta);

							SDK::FVector worldPos = { center.X + x, center.Y + y, center.Z + z };
							SDK::FVector2D screenPos;
							bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

							if (valid && prevValid) {
								pDrawList->AddLine(ImVec2(prevScreen.X, prevScreen.Y), ImVec2(screenPos.X, screenPos.Y), color, 1.0f);
							}

							prevScreen = screenPos;
							prevValid = valid;
						}
					}

					data = static_cast<void*>(cmd + 1);
					break;
				}

				float pitch = rotation.Pitch * (3.14159f / 180.0f);
				float yaw = rotation.Yaw * (3.14159f / 180.0f);
				float roll = rotation.Roll * (3.14159f / 180.0f);

				float cosPitch = cosf(pitch), sinPitch = sinf(pitch);
				float cosYaw = cosf(yaw), sinYaw = sinf(yaw);
				float cosRoll = cosf(roll), sinRoll = sinf(roll);

				const int latLines = 3;
				const int lonLines = 4;
				const int pointsPerCircle = 18;

				for (int lat = 1; lat < latLines; lat++) {
					float phi = (float)lat * 3.14159f / latLines;
					float y = radius * cosf(phi);
					float circleRadius = radius * sinf(phi);

					SDK::FVector2D prevScreen;
					bool prevValid = false;

					for (int i = 0; i <= pointsPerCircle; i++) {
						float theta = (float)i * 2.0f * 3.14159f / pointsPerCircle;
						float x = circleRadius * cosf(theta);
						float z = circleRadius * sinf(theta);

						float rotX = x * (cosYaw * cosRoll - sinYaw * sinPitch * sinRoll) +
							y * (-cosYaw * sinRoll - sinYaw * sinPitch * cosRoll) +
							z * (-sinYaw * cosPitch);

						float rotY = x * (sinYaw * cosRoll + cosYaw * sinPitch * sinRoll) +
							y * (-sinYaw * sinRoll + cosYaw * sinPitch * cosRoll) +
							z * (cosYaw * cosPitch);

						float rotZ = x * (cosPitch * sinRoll) +
							y * (cosPitch * cosRoll) +
							z * (-sinPitch);

						SDK::FVector worldPos = { center.X + rotX, center.Y + rotY, center.Z + rotZ };
						SDK::FVector2D screenPos;
						bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

						if (valid && prevValid) {
							pDrawList->AddLine(ImVec2(prevScreen.X, prevScreen.Y), ImVec2(screenPos.X, screenPos.Y), color, 2.0f);
						}

						prevScreen = screenPos;
						prevValid = valid;
					}
				}

				for (int lon = 0; lon < lonLines; lon++) {
					float theta = (float)lon * 2.0f * 3.14159f / lonLines;

					SDK::FVector2D prevScreen;
					bool prevValid = false;

					for (int i = 0; i <= pointsPerCircle; i++) {
						float phi = (float)i * 3.14159f / pointsPerCircle;
						float x = radius * sinf(phi) * cosf(theta);
						float y = radius * cosf(phi);
						float z = radius * sinf(phi) * sinf(theta);

						float rotX = x * (cosYaw * cosRoll - sinYaw * sinPitch * sinRoll) +
							y * (-cosYaw * sinRoll - sinYaw * sinPitch * cosRoll) +
							z * (-sinYaw * cosPitch);

						float rotY = x * (sinYaw * cosRoll + cosYaw * sinPitch * sinRoll) +
							y * (-sinYaw * sinRoll + cosYaw * sinPitch * cosRoll) +
							z * (cosYaw * cosPitch);

						float rotZ = x * (cosPitch * sinRoll) +
							y * (cosPitch * cosRoll) +
							z * (-sinPitch);

						SDK::FVector worldPos = { center.X + rotX, center.Y + rotY, center.Z + rotZ };
						SDK::FVector2D screenPos;
						bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

						if (valid && prevValid) {
							pDrawList->AddLine(ImVec2(prevScreen.X, prevScreen.Y), ImVec2(screenPos.X, screenPos.Y), color, 2.0f);
						}

						prevScreen = screenPos;
						prevValid = valid;
					}
				}
				const int beamSegments = 8;
				const int beamRays = 7;

				for (int ray = 0; ray < beamRays; ray++) {
					float rayAngle = (float)ray * 2.0f * 3.14159f / beamRays;

					SDK::FVector2D prevScreen;
					bool prevValid = false;

					for (int seg = 0; seg <= beamSegments; seg++) {
						float t = (float)seg / beamSegments;
						float height = t * beamHeight;

						float beamRadius = radius * 0.3f * (1.0f + t * 2.0f);
						float x = beamRadius * cosf(rayAngle);
						float y = beamRadius * sinf(rayAngle);
						float rotX = x * (cosYaw * cosRoll - sinYaw * sinPitch * sinRoll) +
							height * (-cosYaw * sinRoll - sinYaw * sinPitch * cosRoll) +
							y * (-sinYaw * cosPitch);

						float rotY = x * (sinYaw * cosRoll + cosYaw * sinPitch * sinRoll) +
							height * (-sinYaw * sinRoll + cosYaw * sinPitch * cosRoll) +
							y * (cosYaw * cosPitch);

						float rotZ = x * (cosPitch * sinRoll) +
							height * (cosPitch * cosRoll) +
							y * (-sinPitch);

						SDK::FVector worldPos = { center.X + rotX, center.Y + rotY, center.Z + radius + rotZ };
						SDK::FVector2D screenPos;
						bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

						if (valid && prevValid) {
							static float lightTime = 0.0f;
							lightTime += 0.05f;
							float lightVariation = 0.8f + 0.2f * sinf(lightTime + ray * 0.3f);

							float alpha = (1.0f - t) * beamIntensity * color.Value.w * lightVariation;
							ImColor beamColor(color.Value.x, color.Value.y, color.Value.z, alpha);
							pDrawList->AddLine(ImVec2(prevScreen.X, prevScreen.Y), ImVec2(screenPos.X, screenPos.Y), beamColor, 3.5f * (1.0f - t * 0.6f));
						}

						prevScreen = screenPos;
						prevValid = valid;
					}
				}
				for (int ray = 0; ray < beamRays; ray++) {
					float rayAngle = (float)ray * 2.0f * 3.14159f / beamRays;

					SDK::FVector2D prevScreen;
					bool prevValid = false;

					for (int seg = 0; seg <= beamSegments; seg++) {
						float t = (float)seg / beamSegments;
						float height = -t * beamHeight;

						float beamRadius = radius * 0.3f * (1.0f + t * 2.0f);
						float x = beamRadius * cosf(rayAngle);
						float y = beamRadius * sinf(rayAngle);
						float rotX = x * (cosYaw * cosRoll - sinYaw * sinPitch * sinRoll) +
							height * (-cosYaw * sinRoll - sinYaw * sinPitch * cosRoll) +
							y * (-sinYaw * cosPitch);

						float rotY = x * (sinYaw * cosRoll + cosYaw * sinPitch * sinRoll) +
							height * (-sinYaw * sinRoll + cosYaw * sinPitch * cosRoll) +
							y * (cosYaw * cosPitch);

						float rotZ = x * (cosPitch * sinRoll) +
							height * (cosPitch * cosRoll) +
							y * (-sinPitch);

						SDK::FVector worldPos = { center.X + rotX, center.Y + rotY, center.Z - radius + rotZ };
						SDK::FVector2D screenPos;
						bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

						if (valid && prevValid) {
							static float lightTime = 0.0f;
							lightTime += 0.05f;
							float lightVariation = 0.8f + 0.2f * sinf(lightTime + ray * 0.3f);

							float alpha = (1.0f - t) * beamIntensity * color.Value.w * lightVariation;
							ImColor beamColor(color.Value.x, color.Value.y, color.Value.z, alpha);
							pDrawList->AddLine(ImVec2(prevScreen.X, prevScreen.Y), ImVec2(screenPos.X, screenPos.Y), beamColor, 3.5f * (1.0f - t * 0.6f));
						}

						prevScreen = screenPos;
						prevValid = valid;
					}
				}

				const int glowRings = 4;
				for (int ring = 0; ring < glowRings; ring++) {
					float glowRadius = radius * (1.0f + (ring + 1) * 0.15f);
					float glowAlpha = (1.0f - (float)ring / glowRings) * 0.3f * beamIntensity;

					for (int i = 0; i < 16; i++) {
						float angle = (float)i * 2.0f * 3.14159f / 16;
						float x = glowRadius * cosf(angle);
						float y = glowRadius * sinf(angle);

						float rotX = x * (cosYaw * cosRoll - sinYaw * sinPitch * sinRoll) +
							0 * (-cosYaw * sinRoll - sinYaw * sinPitch * cosRoll) +
							y * (-sinYaw * cosPitch);

						float rotY = x * (sinYaw * cosRoll + cosYaw * sinPitch * sinRoll) +
							0 * (-sinYaw * sinRoll + cosYaw * sinPitch * cosRoll) +
							y * (cosYaw * cosPitch);

						float rotZ = x * (cosPitch * sinRoll) +
							0 * (cosPitch * cosRoll) +
							y * (-sinPitch);

						SDK::FVector worldPos = { center.X + rotX, center.Y + rotY, center.Z + rotZ };
						SDK::FVector2D screenPos;
						bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

						if (valid) {
							ImColor glowColor(color.Value.x, color.Value.y, color.Value.z, glowAlpha);
							pDrawList->AddCircleFilled(ImVec2(screenPos.X, screenPos.Y), 2.0f, glowColor);
						}
					}
				}

				data = static_cast<void*>(cmd + 1);
				break;
			}
			case RD_3DSPHERE_LIGHTBEAM_GRADIENT:
			{
				rd_3dsphere_lightbeam_gradient* cmd = static_cast<rd_3dsphere_lightbeam_gradient*>(data);
				SDK::FVector center(cmd->center[0], cmd->center[1], cmd->center[2]);
				float radius = cmd->radius;
				SDK::FRotator rotation(cmd->rotation[0], cmd->rotation[1], cmd->rotation[2]);
				float beamHeight = cmd->beamHeight;
				float beamIntensity = cmd->beamIntensity;
				SDK::FLinearColor color1(cmd->color1[0], cmd->color1[1], cmd->color1[2], cmd->color1[3]);
				SDK::FLinearColor color2(cmd->color2[0], cmd->color2[1], cmd->color2[2], cmd->color2[3]);

				static float gradientTime = 0.0f;
				gradientTime += 0.016f;

				float distanceFromPlayer = 0.0f;
				bool useSimpleRender = false;

				if (Cheat::LocalPlayerCharacter) {
					SDK::FVector playerPos = Cheat::LocalPlayerCharacter->K2_GetActorLocation();
					distanceFromPlayer = center.GetDistanceTo(playerPos) / 100.0f;
					useSimpleRender = distanceFromPlayer > 50.0f;
				}

				if (useSimpleRender) {
					const int simpleLatLines = 2;
					const int simplePointsPerCircle = 8;

					for (int lat = 1; lat < simpleLatLines; lat++) {
						float phi = (float)lat * 3.14159f / simpleLatLines;
						float y = radius * cosf(phi);
						float circleRadius = radius * sinf(phi);

						float heightT = (float)lat / simpleLatLines;
						SDK::FLinearColor lineColor = GetFlowingRainbowColor(heightT, gradientTime);
						ImColor lineImColor(lineColor.R, lineColor.G, lineColor.B, lineColor.A);

						SDK::FVector2D prevScreen;
						bool prevValid = false;

						for (int i = 0; i <= simplePointsPerCircle; i++) {
							float theta = (float)i * 2.0f * 3.14159f / simplePointsPerCircle;
							float x = circleRadius * cosf(theta);
							float z = circleRadius * sinf(theta);

							SDK::FVector worldPos = { center.X + x, center.Y + y, center.Z + z };
							SDK::FVector2D screenPos;
							bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

							if (valid && prevValid) {
								pDrawList->AddLine(ImVec2(prevScreen.X, prevScreen.Y), ImVec2(screenPos.X, screenPos.Y), lineImColor, 1.0f);
							}

							prevScreen = screenPos;
							prevValid = valid;
						}
					}

					const int simpleLonLines = 3;
					for (int lon = 0; lon < simpleLonLines; lon++) {
						float theta = (float)lon * 2.0f * 3.14159f / simpleLonLines;
						float lonPosition = (float)lon / simpleLonLines;
						SDK::FLinearColor lineColor = GetFlowingRainbowColor(lonPosition, gradientTime);
						ImColor lineImColor(lineColor.R, lineColor.G, lineColor.B, lineColor.A);

						SDK::FVector2D prevScreen;
						bool prevValid = false;

						for (int i = 0; i <= simplePointsPerCircle; i++) {
							float phi = (float)i * 3.14159f / simplePointsPerCircle;
							float x = radius * sinf(phi) * cosf(theta);
							float y = radius * cosf(phi);
							float z = radius * sinf(phi) * sinf(theta);

							SDK::FVector worldPos = { center.X + x, center.Y + y, center.Z + z };
							SDK::FVector2D screenPos;
							bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

							if (valid && prevValid) {
								pDrawList->AddLine(ImVec2(prevScreen.X, prevScreen.Y), ImVec2(screenPos.X, screenPos.Y), lineImColor, 1.0f);
							}

							prevScreen = screenPos;
							prevValid = valid;
						}
					}

					data = static_cast<void*>(cmd + 1);
					break;
				}

				float pitch = rotation.Pitch * (3.14159f / 180.0f);
				float yaw = rotation.Yaw * (3.14159f / 180.0f);
				float roll = rotation.Roll * (3.14159f / 180.0f);

				float cosPitch = cosf(pitch), sinPitch = sinf(pitch);
				float cosYaw = cosf(yaw), sinYaw = sinf(yaw);
				float cosRoll = cosf(roll), sinRoll = sinf(roll);

				const int latLines = 3;
				const int lonLines = 4;
				const int pointsPerCircle = 18;

				for (int lat = 1; lat < latLines; lat++) {
					float phi = (float)lat * 3.14159f / latLines;
					float y = radius * cosf(phi);
					float circleRadius = radius * sinf(phi);

					float heightT = (float)lat / latLines;
					SDK::FLinearColor lineColor = GetFlowingRainbowColor(heightT, gradientTime);
					ImColor lineImColor(lineColor.R, lineColor.G, lineColor.B, lineColor.A);

					SDK::FVector2D prevScreen;
					bool prevValid = false;

					for (int i = 0; i <= pointsPerCircle; i++) {
						float theta = (float)i * 2.0f * 3.14159f / pointsPerCircle;
						float x = circleRadius * cosf(theta);
						float z = circleRadius * sinf(theta);

						float rotX = x * (cosYaw * cosRoll - sinYaw * sinPitch * sinRoll) +
							y * (-cosYaw * sinRoll - sinYaw * sinPitch * cosRoll) +
							z * (-sinYaw * cosPitch);

						float rotY = x * (sinYaw * cosRoll + cosYaw * sinPitch * sinRoll) +
							y * (-sinYaw * sinRoll + cosYaw * sinPitch * cosRoll) +
							z * (cosYaw * cosPitch);

						float rotZ = x * (cosPitch * sinRoll) +
							y * (cosPitch * cosRoll) +
							z * (-sinPitch);

						SDK::FVector worldPos = { center.X + rotX, center.Y + rotY, center.Z + rotZ };
						SDK::FVector2D screenPos;
						bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

						if (valid && prevValid) {
							pDrawList->AddLine(ImVec2(prevScreen.X, prevScreen.Y), ImVec2(screenPos.X, screenPos.Y), lineImColor, 2.0f);
						}

						prevScreen = screenPos;
						prevValid = valid;
					}
				}

				for (int lon = 0; lon < lonLines; lon++) {
					float theta = (float)lon * 2.0f * 3.14159f / lonLines;
					float lonPosition = (float)lon / lonLines;
					SDK::FLinearColor lineColor = GetFlowingRainbowColor(lonPosition + 0.5f, gradientTime);
					ImColor lineImColor(lineColor.R, lineColor.G, lineColor.B, lineColor.A);

					SDK::FVector2D prevScreen;
					bool prevValid = false;

					for (int i = 0; i <= pointsPerCircle; i++) {
						float phi = (float)i * 3.14159f / pointsPerCircle;
						float x = radius * sinf(phi) * cosf(theta);
						float y = radius * cosf(phi);
						float z = radius * sinf(phi) * sinf(theta);

						float rotX = x * (cosYaw * cosRoll - sinYaw * sinPitch * sinRoll) +
							y * (-cosYaw * sinRoll - sinYaw * sinPitch * cosRoll) +
							z * (-sinYaw * cosPitch);

						float rotY = x * (sinYaw * cosRoll + cosYaw * sinPitch * sinRoll) +
							y * (-sinYaw * sinRoll + cosYaw * sinPitch * cosRoll) +
							z * (cosYaw * cosPitch);

						float rotZ = x * (cosPitch * sinRoll) +
							y * (cosPitch * cosRoll) +
							z * (-sinPitch);

						SDK::FVector worldPos = { center.X + rotX, center.Y + rotY, center.Z + rotZ };
						SDK::FVector2D screenPos;
						bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

						if (valid && prevValid) {
							pDrawList->AddLine(ImVec2(prevScreen.X, prevScreen.Y), ImVec2(screenPos.X, screenPos.Y), lineImColor, 2.0f);
						}

						prevScreen = screenPos;
						prevValid = valid;
					}
				}

				const int beamSegments = 8;
				const int beamRays = 7;

				for (int ray = 0; ray < beamRays; ray++) {
					float rayAngle = (float)ray * 2.0f * 3.14159f / beamRays;

					SDK::FVector2D prevScreen;
					bool prevValid = false;

					for (int seg = 0; seg <= beamSegments; seg++) {
						float t = (float)seg / beamSegments;
						float height = t * beamHeight;

						float beamPosition = (float)ray / beamRays + t * 0.3f;
						SDK::FLinearColor beamColor = GetFlowingRainbowColor(beamPosition, gradientTime);

						float beamRadius = radius * 0.3f * (1.0f + t * 2.0f);
						float x = beamRadius * cosf(rayAngle);
						float y = beamRadius * sinf(rayAngle);
						float rotX = x * (cosYaw * cosRoll - sinYaw * sinPitch * sinRoll) +
							height * (-cosYaw * sinRoll - sinYaw * sinPitch * cosRoll) +
							y * (-sinYaw * cosPitch);

						float rotY = x * (sinYaw * cosRoll + cosYaw * sinPitch * sinRoll) +
							height * (-sinYaw * sinRoll + cosYaw * sinPitch * cosRoll) +
							y * (cosYaw * cosPitch);

						float rotZ = x * (cosPitch * sinRoll) +
							height * (cosPitch * cosRoll) +
							y * (-sinPitch);

						SDK::FVector worldPos = { center.X + rotX, center.Y + rotY, center.Z + radius + rotZ };
						SDK::FVector2D screenPos;
						bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

						if (valid && prevValid) {
							static float lightTime = 0.0f;
							lightTime += 0.05f;
							float lightVariation = 0.8f + 0.2f * sinf(lightTime + ray * 0.3f);

							float alpha = (1.0f - t) * beamIntensity * beamColor.A * lightVariation;
							ImColor finalBeamColor(beamColor.R, beamColor.G, beamColor.B, alpha);
							pDrawList->AddLine(ImVec2(prevScreen.X, prevScreen.Y), ImVec2(screenPos.X, screenPos.Y), finalBeamColor, 3.5f * (1.0f - t * 0.6f));
						}

						prevScreen = screenPos;
						prevValid = valid;
					}
				}

				for (int ray = 0; ray < beamRays; ray++) {
					float rayAngle = (float)ray * 2.0f * 3.14159f / beamRays;

					SDK::FVector2D prevScreen;
					bool prevValid = false;

					for (int seg = 0; seg <= beamSegments; seg++) {
						float t = (float)seg / beamSegments;
						float height = -t * beamHeight;

						float beamPosition = (float)ray / beamRays + t * 0.3f + 0.5f;
						SDK::FLinearColor beamColor = GetFlowingRainbowColor(beamPosition, gradientTime);

						float beamRadius = radius * 0.3f * (1.0f + t * 2.0f);
						float x = beamRadius * cosf(rayAngle);
						float y = beamRadius * sinf(rayAngle);
						float rotX = x * (cosYaw * cosRoll - sinYaw * sinPitch * sinRoll) +
							height * (-cosYaw * sinRoll - sinYaw * sinPitch * cosRoll) +
							y * (-sinYaw * cosPitch);

						float rotY = x * (sinYaw * cosRoll + cosYaw * sinPitch * sinRoll) +
							height * (-sinYaw * sinRoll + cosYaw * sinPitch * cosRoll) +
							y * (cosYaw * cosPitch);

						float rotZ = x * (cosPitch * sinRoll) +
							height * (cosPitch * cosRoll) +
							y * (-sinPitch);

						SDK::FVector worldPos = { center.X + rotX, center.Y + rotY, center.Z - radius + rotZ };
						SDK::FVector2D screenPos;
						bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

						if (valid && prevValid) {
							static float lightTime = 0.0f;
							lightTime += 0.05f;
							float lightVariation = 0.8f + 0.2f * sinf(lightTime + ray * 0.3f);

							float alpha = (1.0f - t) * beamIntensity * beamColor.A * lightVariation;
							ImColor finalBeamColor(beamColor.R, beamColor.G, beamColor.B, alpha);
							pDrawList->AddLine(ImVec2(prevScreen.X, prevScreen.Y), ImVec2(screenPos.X, screenPos.Y), finalBeamColor, 3.5f * (1.0f - t * 0.6f));
						}

						prevScreen = screenPos;
						prevValid = valid;
					}
				}

				const int glowRings = 4;
				for (int ring = 0; ring < glowRings; ring++) {
					float glowRadius = radius * (1.0f + (ring + 1) * 0.15f);
					float glowAlpha = (1.0f - (float)ring / glowRings) * 0.3f * beamIntensity;
					float ringPosition = (float)ring / glowRings;
					SDK::FLinearColor glowColor = GetFlowingRainbowColor(ringPosition, gradientTime);

					for (int i = 0; i < 16; i++) {
						float angle = (float)i * 2.0f * 3.14159f / 16;
						float x = glowRadius * cosf(angle);
						float y = glowRadius * sinf(angle);

						float rotX = x * (cosYaw * cosRoll - sinYaw * sinPitch * sinRoll) +
							0 * (-cosYaw * sinRoll - sinYaw * sinPitch * cosRoll) +
							y * (-sinYaw * cosPitch);

						float rotY = x * (sinYaw * cosRoll + cosYaw * sinPitch * sinRoll) +
							0 * (-sinYaw * sinRoll + cosYaw * sinPitch * cosRoll) +
							y * (cosYaw * cosPitch);

						float rotZ = x * (cosPitch * sinRoll) +
							0 * (cosPitch * cosRoll) +
							y * (-sinPitch);

						SDK::FVector worldPos = { center.X + rotX, center.Y + rotY, center.Z + rotZ };
						SDK::FVector2D screenPos;
						bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

						if (valid) {
							ImColor finalGlowColor(glowColor.R, glowColor.G, glowColor.B, glowAlpha);
							pDrawList->AddCircleFilled(ImVec2(screenPos.X, screenPos.Y), 2.0f, finalGlowColor);
						}
					}
				}

				data = static_cast<void*>(cmd + 1);
				break;
			}
			case RD_3DBOX_GRADIENT:
			{
				rd_3dbox_gradient* cmd = static_cast<rd_3dbox_gradient*>(data);
				SDK::FVector center(cmd->center[0], cmd->center[1], cmd->center[2]);
				SDK::FVector extent(cmd->extent[0], cmd->extent[1], cmd->extent[2]);
				SDK::FLinearColor color1(cmd->color1[0], cmd->color1[1], cmd->color1[2], cmd->color1[3]);
				SDK::FLinearColor color2(cmd->color2[0], cmd->color2[1], cmd->color2[2], cmd->color2[3]);

				SDK::FVector corners[8] = {
					{center.X - extent.X, center.Y - extent.Y, center.Z - extent.Z},   
					{center.X + extent.X, center.Y - extent.Y, center.Z - extent.Z},
					{center.X + extent.X, center.Y + extent.Y, center.Z - extent.Z},
					{center.X - extent.X, center.Y + extent.Y, center.Z - extent.Z},
					{center.X - extent.X, center.Y - extent.Y, center.Z + extent.Z},   
					{center.X + extent.X, center.Y - extent.Y, center.Z + extent.Z},
					{center.X + extent.X, center.Y + extent.Y, center.Z + extent.Z},
					{center.X - extent.X, center.Y + extent.Y, center.Z + extent.Z}
				};

				SDK::FVector2D screenCorners[8];
				bool validCorners[8];
				int validCount = 0;

				for (int i = 0; i < 8; i++) {
					validCorners[i] = Cheat::WorldToScreen(corners[i], &screenCorners[i]);
					if (validCorners[i]) validCount++;
				}

				if (validCount > 0) {
					static float gradientTime = 0.0f;
					gradientTime += 0.016f;  

					int edges[12][2] = {
						{0,1}, {1,2}, {2,3}, {3,0},   
						{4,5}, {5,6}, {6,7}, {7,4},     
						{0,4}, {1,5}, {2,6}, {3,7}    
					};

					for (int edge = 0; edge < 12; edge++) {
						int v1 = edges[edge][0];
						int v2 = edges[edge][1]; RD_3DSPHERE_GRADIENT;

						if (validCorners[v1] && validCorners[v2]) {
							const int edgeSegments = 15;
							for (int i = 0; i < edgeSegments; i++) {
								float t = (float)i / edgeSegments;
								float x1 = screenCorners[v1].X + (screenCorners[v2].X - screenCorners[v1].X) * t;
								float y1 = screenCorners[v1].Y + (screenCorners[v2].Y - screenCorners[v1].Y) * t;
								float x2 = screenCorners[v1].X + (screenCorners[v2].X - screenCorners[v1].X) * (t + 1.0f / edgeSegments);
								float y2 = screenCorners[v1].Y + (screenCorners[v2].Y - screenCorners[v1].Y) * (t + 1.0f / edgeSegments);

								float edgePosition = (float)edge / 12.0f + t * 0.5f;
								SDK::FLinearColor edgeColor = GetFlowingRainbowColor(edgePosition, gradientTime);
								ImColor edgeImColor(edgeColor.R, edgeColor.G, edgeColor.B, edgeColor.A);

								pDrawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), edgeImColor, 2.0f);
							}
						}
					}
				}

				data = static_cast<void*>(cmd + 1);
				break;
			}
			case RD_3DSPHERE_GRADIENT:
			{
				rd_3dsphere_gradient* cmd = static_cast<rd_3dsphere_gradient*>(data);
				SDK::FVector center(cmd->center[0], cmd->center[1], cmd->center[2]);
				float radius = cmd->radius;
				SDK::FLinearColor color1(cmd->color1[0], cmd->color1[1], cmd->color1[2], cmd->color1[3]);
				SDK::FLinearColor color2(cmd->color2[0], cmd->color2[1], cmd->color2[2], cmd->color2[3]);

				static float gradientTime = 0.0f;
				gradientTime += 0.016f;

				const int latLines = 3;
				const int lonLines = 4;
				const int pointsPerCircle = 18;

				for (int lat = 1; lat < latLines; lat++) {
					float phi = (float)lat * 3.14159f / latLines;
					float y = radius * cosf(phi);
					float circleRadius = radius * sinf(phi);

					float heightT = (y + radius) / (2.0f * radius);       
					SDK::FLinearColor lineColor = GetFlowingRainbowColor(heightT, gradientTime);
					ImColor lineImColor(lineColor.R, lineColor.G, lineColor.B, lineColor.A);

					SDK::FVector2D prevScreen;
					bool prevValid = false;

					for (int i = 0; i <= pointsPerCircle; i++) {
						float theta = (float)i * 2.0f * 3.14159f / pointsPerCircle;
						float x = circleRadius * cosf(theta);
						float z = circleRadius * sinf(theta);

						SDK::FVector worldPos = { center.X + x, center.Y + z, center.Z + y };
						SDK::FVector2D screenPos;
						bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

						if (valid && prevValid) {
							pDrawList->AddLine(ImVec2(prevScreen.X, prevScreen.Y), ImVec2(screenPos.X, screenPos.Y), lineImColor, 1.0f);
						}

						prevScreen = screenPos;
						prevValid = valid;
					}
				}

				for (int lon = 0; lon < lonLines; lon++) {
					float theta = (float)lon * 2.0f * 3.14159f / lonLines;

					SDK::FVector2D prevScreen;
					bool prevValid = false;

					for (int i = 0; i <= pointsPerCircle; i++) {
						float phi = (float)i * 3.14159f / pointsPerCircle;
						float x = radius * sinf(phi) * cosf(theta);
						float y = radius * cosf(phi);
						float z = radius * sinf(phi) * sinf(theta);

						float heightT = (y + radius) / (2.0f * radius);
						float lonPosition = (float)lon / lonLines;
						float combinedPosition = heightT * 0.7f + lonPosition * 0.3f;
						SDK::FLinearColor segmentColor = GetFlowingRainbowColor(combinedPosition, gradientTime);
						ImColor segImColor(segmentColor.R, segmentColor.G, segmentColor.B, segmentColor.A);

						SDK::FVector worldPos = { center.X + x, center.Y + z, center.Z + y };
						SDK::FVector2D screenPos;
						bool valid = Cheat::WorldToScreen(worldPos, &screenPos);

						if (valid && prevValid) {
							pDrawList->AddLine(ImVec2(prevScreen.X, prevScreen.Y), ImVec2(screenPos.X, screenPos.Y), segImColor, 1.0f);
						}

						prevScreen = screenPos;
						prevValid = valid;
					}
				}

				data = static_cast<void*>(cmd + 1);
				break;
			}
			case RD_CIRCLE_GRADIENT:
			{
				rd_circle_gradient* cmd = static_cast<rd_circle_gradient*>(data);
				SDK::FLinearColor color1(cmd->color1[0], cmd->color1[1], cmd->color1[2], cmd->color1[3]);
				SDK::FLinearColor color2(cmd->color2[0], cmd->color2[1], cmd->color2[2], cmd->color2[3]);

				static float gradientTime = 0.0f;
				gradientTime += 0.016f;

				if (cmd->radius > 0.0f &&
					cmd->x - cmd->radius < io.DisplaySize.x &&
					cmd->x + cmd->radius > 0 &&
					cmd->y - cmd->radius < io.DisplaySize.y &&
					cmd->y + cmd->radius > 0)
				{
					const int segments = 24;
					for (int i = 0; i < segments; i++) {
						float angle1 = (float)i * 2.0f * 3.14159f / segments;
						float angle2 = (float)(i + 1) * 2.0f * 3.14159f / segments;

						float t = (float)i / segments;
						SDK::FLinearColor segmentColor = GetFlowingRainbowColor(t, gradientTime);
						ImColor segImColor(segmentColor.R, segmentColor.G, segmentColor.B, segmentColor.A);

						ImVec2 pos1(cmd->x + cmd->radius * cosf(angle1), cmd->y + cmd->radius * sinf(angle1));
						ImVec2 pos2(cmd->x + cmd->radius * cosf(angle2), cmd->y + cmd->radius * sinf(angle2));
						pDrawList->AddLine(pos1, pos2, segImColor, 1.5f);
					}
				}

				data = static_cast<void*>(cmd + 1);
				break;
			}
			case RD_RECT_GRADIENT:
			{
				rd_rect_gradient* cmd = static_cast<rd_rect_gradient*>(data);
				SDK::FLinearColor color1(cmd->color1[0], cmd->color1[1], cmd->color1[2], cmd->color1[3]);
				SDK::FLinearColor color2(cmd->color2[0], cmd->color2[1], cmd->color2[2], cmd->color2[3]);

				static float gradientTime = 0.0f;
				gradientTime += 0.016f;

				const int segments = 20;
				for (int i = 0; i < segments; i++) {
					float t = (float)i / segments;
					float y1 = cmd->y + (cmd->h * t);
					float y2 = cmd->y + (cmd->h * (t + 1.0f / segments));

					SDK::FLinearColor segmentColor = GetFlowingRainbowColor(t, gradientTime);
					ImColor segImColor(segmentColor.R, segmentColor.G, segmentColor.B, segmentColor.A);

					pDrawList->AddRectFilled(ImVec2(cmd->x, y1), ImVec2(cmd->x + cmd->w, y2), segImColor);
				}

				SDK::FLinearColor outlineColor = GetFlowingRainbowColor(0.0f, gradientTime);
				ImColor outlineImColor(outlineColor.R, outlineColor.G, outlineColor.B, outlineColor.A);
				pDrawList->AddRect(ImVec2(cmd->x, cmd->y), ImVec2(cmd->x + cmd->w, cmd->y + cmd->h), outlineImColor, 0.0f, 0, 2.0f);

				data = static_cast<void*>(cmd + 1);
				break;
			}
			case RD_LINE_GRADIENT:
			{
				rd_line_gradient* cmd = static_cast<rd_line_gradient*>(data);
				SDK::FLinearColor color1(cmd->color1[0], cmd->color1[1], cmd->color1[2], cmd->color1[3]);
				SDK::FLinearColor color2(cmd->color2[0], cmd->color2[1], cmd->color2[2], cmd->color2[3]);

				static float gradientTime = 0.0f;
				gradientTime += 0.016f;

				const int segments = 30;
				for (int i = 0; i < segments; i++) {
					float t = (float)i / segments;
					float x1 = cmd->x + (cmd->x2 - cmd->x) * t;
					float y1 = cmd->y + (cmd->y2 - cmd->y) * t;
					float x2 = cmd->x + (cmd->x2 - cmd->x) * (t + 1.0f / segments);
					float y2 = cmd->y + (cmd->y2 - cmd->y) * (t + 1.0f / segments);

					SDK::FLinearColor segmentColor = GetFlowingRainbowColor(t, gradientTime);
					ImColor segImColor(segmentColor.R, segmentColor.G, segmentColor.B, segmentColor.A);

					pDrawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), segImColor, 3.0f);
				}

				data = static_cast<void*>(cmd + 1);
				break;
			}
			case RD_TEXT_GRADIENT:
			{
				rd_text_gradient* cmd = static_cast<rd_text_gradient*>(data);
				SDK::FLinearColor color1(cmd->color1[0], cmd->color1[1], cmd->color1[2], cmd->color1[3]);
				SDK::FLinearColor color2(cmd->color2[0], cmd->color2[1], cmd->color2[2], cmd->color2[3]);

				static float gradientTime = 0.0f;
				gradientTime += 0.016f;

				std::string text = cmd->txt;
				float charX = cmd->x;

				if (cmd->bg) {
					ImVec2 textSize = ImGui::CalcTextSize(cmd->txt);
					pDrawList->AddRectFilled(ImVec2(cmd->x - 2, cmd->y - 2),
						ImVec2(cmd->x + textSize.x + 2, cmd->y + textSize.y + 2),
						ImColor(0, 0, 0, 180));
				}

				for (size_t i = 0; i < text.length(); i++) {
					float charPosition = (float)i / text.length();
					SDK::FLinearColor charColor = GetFlowingRainbowColor(charPosition, gradientTime);
					ImColor charImColor(charColor.R, charColor.G, charColor.B, charColor.A);

					char charStr[2] = { text[i], '\0' };
					pDrawList->AddText(ImVec2(charX, cmd->y), charImColor, charStr);

					ImVec2 charSize = ImGui::CalcTextSize(charStr);
					charX += charSize.x;
				}

				data = static_cast<void*>(cmd + 1);
				break;
			}
			case RD_END:
			default:
				isDone = true;
				break;
			}
		}
		ImGui::End();

	}

	static void* R_GetCmdBuffer(int bytes)
	{
		rd_Commands* cmdList = &Commands;
		if (!cmdList)
		{
			return nullptr;
		}

		bytes = PAD(bytes, sizeof(void*));

		if (cmdList->used + bytes > sizeof(cmdList->cmds))
		{
			return nullptr;
		}
		void* buffer = cmdList->cmds + cmdList->used;
		cmdList->used += bytes;

		return buffer;
	}

	void R_DrawText(float x, float y, UC::FString text, SDK::FLinearColor* color, bool background)
	{
		void* buffer = R_GetCmdBuffer(sizeof(rd_text));
		if (!buffer)
		{
			return;
		}
		rd_text* cmd = static_cast<rd_text*>(buffer);
		if (!cmd)
		{
			cmd->cmdID = RD_END;
			return;
		}
		cmd->cmdID = RD_TEXT;
		cmd->x = x;
		cmd->y = y;
		std::string str = text.ToString();
		str.copy(cmd->txt, sizeof(cmd->txt) - 1);
		cmd->txt[str.size()] = '\0';
		cmd->color[0] = color->R;
		cmd->color[1] = color->G;
		cmd->color[2] = color->B;
		cmd->color[3] = color->A;
		cmd->bg = background;
	}

	void R_DrawText(float x, float y, std::string text, SDK::FLinearColor* color, bool background)
	{
		void* buffer = R_GetCmdBuffer(sizeof(rd_text));
		if (!buffer)
		{
			return;
		}
		rd_text* cmd = static_cast<rd_text*>(buffer);
		if (!cmd)
		{
			cmd->cmdID = RD_END;
			return;
		}
		cmd->cmdID = RD_TEXT;
		cmd->x = x;
		cmd->y = y;
		std::string str = text;
		str.copy(cmd->txt, sizeof(cmd->txt) - 1);
		cmd->txt[str.size()] = '\0';
		cmd->color[0] = color->R;
		cmd->color[1] = color->G;
		cmd->color[2] = color->B;
		cmd->color[3] = color->A;
		cmd->bg = background;
	}

	void R_DrawRect(float x, float y, float w, float h, SDK::FLinearColor* color)
	{
		void* buffer = R_GetCmdBuffer(sizeof(rd_rect));
		if (!buffer)
		{
			return;
		}
		rd_rect* cmd = static_cast<rd_rect*>(buffer);
		if (!cmd)
		{
			cmd->cmdID = RD_END;
			return;
		}
		cmd->cmdID = RD_RECT;
		cmd->x = x;
		cmd->y = y;
		cmd->w = w;
		cmd->h = h;
		cmd->color[0] = color->R;
		cmd->color[1] = color->G;
		cmd->color[2] = color->B;
		cmd->color[3] = color->A;
	}

	void R_DrawFillRect(float x, float y, float w, float h, SDK::FLinearColor* color)
	{
		rd_fillrect* cmd;
		cmd = static_cast<rd_fillrect*>(R_GetCmdBuffer(sizeof(*cmd)));
		if (!cmd)
			return;
		cmd->cmdID = RD_FILLRECT;
		cmd->x = x;
		cmd->y = y;
		cmd->w = w;
		cmd->h = h;
		cmd->color[0] = color->R;
		cmd->color[1] = color->G;
		cmd->color[2] = color->B;
		cmd->color[3] = color->A;
	}

	void R_DrawLine(float x, float y, float x2, float y2, SDK::FLinearColor* color)
	{
		rd_line* cmd;
		cmd = static_cast<rd_line*>(R_GetCmdBuffer(sizeof(*cmd)));
		if (!cmd)
			return;
		cmd->cmdID = RD_LINE;
		cmd->x = x;
		cmd->y = y;
		cmd->x2 = x2;
		cmd->y2 = y2;
		cmd->color[0] = color->R;
		cmd->color[1] = color->G;
		cmd->color[2] = color->B;
		cmd->color[3] = color->A;
	}

	void R_DrawCircle(float x, float y, float radius, SDK::FLinearColor* color)
	{
		rd_circle* cmd = static_cast<rd_circle*>(R_GetCmdBuffer(sizeof(*cmd)));
		if (!cmd)
			return;
		cmd->cmdID = RD_CIRCLE;
		cmd->x = x;
		cmd->y = y;
		cmd->radius = radius;
		cmd->color[0] = color->R;
		cmd->color[1] = color->G;
		cmd->color[2] = color->B;
		cmd->color[3] = color->A;
	}

	void R_Draw3DBox(const SDK::FVector& center, const SDK::FVector& extent, SDK::FLinearColor* color)
	{
		rd_3dbox* cmd = static_cast<rd_3dbox*>(R_GetCmdBuffer(sizeof(*cmd)));
		if (!cmd)
			return;
		cmd->cmdID = RD_3DBOX;
		cmd->center[0] = center.X;
		cmd->center[1] = center.Y;
		cmd->center[2] = center.Z;
		cmd->extent[0] = extent.X;
		cmd->extent[1] = extent.Y;
		cmd->extent[2] = extent.Z;
		cmd->color[0] = color->R;
		cmd->color[1] = color->G;
		cmd->color[2] = color->B;
		cmd->color[3] = color->A;
	}

	void R_Draw3DBoxRotated(const SDK::FVector& center, const SDK::FVector& extent, const SDK::FRotator& rotation, SDK::FLinearColor* color)
	{
		rd_3dbox_rotated* cmd = static_cast<rd_3dbox_rotated*>(R_GetCmdBuffer(sizeof(*cmd)));
		if (!cmd)
			return;
		cmd->cmdID = RD_3DBOX_ROTATED;
		cmd->center[0] = center.X;
		cmd->center[1] = center.Y;
		cmd->center[2] = center.Z;
		cmd->extent[0] = extent.X;
		cmd->extent[1] = extent.Y;
		cmd->extent[2] = extent.Z;
		cmd->rotation[0] = rotation.Pitch;
		cmd->rotation[1] = rotation.Yaw;
		cmd->rotation[2] = rotation.Roll;
		cmd->color[0] = color->R;
		cmd->color[1] = color->G;
		cmd->color[2] = color->B;
		cmd->color[3] = color->A;
	}

	void R_Draw3DSphere(const SDK::FVector& center, float radius, SDK::FLinearColor* color)
	{
		rd_3dsphere* cmd = static_cast<rd_3dsphere*>(R_GetCmdBuffer(sizeof(*cmd)));
		if (!cmd)
			return;
		cmd->cmdID = RD_3DSPHERE;
		cmd->center[0] = center.X;
		cmd->center[1] = center.Y;
		cmd->center[2] = center.Z;
		cmd->radius = radius;
		cmd->color[0] = color->R;
		cmd->color[1] = color->G;
		cmd->color[2] = color->B;
		cmd->color[3] = color->A;
	}

	void R_Draw3DSphereRotated(const SDK::FVector& center, float radius, const SDK::FRotator& rotation, SDK::FLinearColor* color)
	{
		rd_3dsphere_rotated* cmd = static_cast<rd_3dsphere_rotated*>(R_GetCmdBuffer(sizeof(*cmd)));
		if (!cmd)
			return;
		cmd->cmdID = RD_3DSPHERE_ROTATED;
		cmd->center[0] = center.X;
		cmd->center[1] = center.Y;
		cmd->center[2] = center.Z;
		cmd->radius = radius;
		cmd->rotation[0] = rotation.Pitch;
		cmd->rotation[1] = rotation.Yaw;
		cmd->rotation[2] = rotation.Roll;
		cmd->color[0] = color->R;
		cmd->color[1] = color->G;
		cmd->color[2] = color->B;
		cmd->color[3] = color->A;
	}

	void R_Draw3DSphereLightBeam(const SDK::FVector& center, float radius, const SDK::FRotator& rotation, float beamHeight, float beamIntensity, SDK::FLinearColor* color)
	{
		rd_3dsphere_lightbeam* cmd = static_cast<rd_3dsphere_lightbeam*>(R_GetCmdBuffer(sizeof(*cmd)));
		if (!cmd)
			return;
		cmd->cmdID = RD_3DSPHERE_LIGHTBEAM;
		cmd->center[0] = center.X;
		cmd->center[1] = center.Y;
		cmd->center[2] = center.Z;
		cmd->radius = radius;
		cmd->rotation[0] = rotation.Pitch;
		cmd->rotation[1] = rotation.Yaw;
		cmd->rotation[2] = rotation.Roll;
		cmd->beamHeight = beamHeight;
		cmd->beamIntensity = beamIntensity;
		cmd->color[0] = color->R;
		cmd->color[1] = color->G;
		cmd->color[2] = color->B;
		cmd->color[3] = color->A;
	}

	SDK::FLinearColor GetGradientColor(SDK::FLinearColor* color1, SDK::FLinearColor* color2, float t)
	{
		t = ImClamp(t, 0.0f, 1.0f);
		SDK::FLinearColor result;
		result.R = color1->R + (color2->R - color1->R) * t;
		result.G = color1->G + (color2->G - color1->G) * t;
		result.B = color1->B + (color2->B - color1->B) * t;
		result.A = color1->A + (color2->A - color1->A) * t;
		return result;
	}

	void R_Draw3DBoxGradient(const SDK::FVector& center, const SDK::FVector& extent, SDK::FLinearColor* color1, SDK::FLinearColor* color2)
	{
		rd_3dbox_gradient* cmd = static_cast<rd_3dbox_gradient*>(R_GetCmdBuffer(sizeof(*cmd)));
		if (!cmd)
			return;
		cmd->cmdID = RD_3DBOX_GRADIENT;
		cmd->center[0] = center.X;
		cmd->center[1] = center.Y;
		cmd->center[2] = center.Z;
		cmd->extent[0] = extent.X;
		cmd->extent[1] = extent.Y;
		cmd->extent[2] = extent.Z;
		cmd->color1[0] = color1->R;
		cmd->color1[1] = color1->G;
		cmd->color1[2] = color1->B;
		cmd->color1[3] = color1->A;
		cmd->color2[0] = color2->R;
		cmd->color2[1] = color2->G;
		cmd->color2[2] = color2->B;
		cmd->color2[3] = color2->A;
	}

	void R_Draw3DSphereGradient(const SDK::FVector& center, float radius, SDK::FLinearColor* color1, SDK::FLinearColor* color2)
	{
		rd_3dsphere_gradient* cmd = static_cast<rd_3dsphere_gradient*>(R_GetCmdBuffer(sizeof(*cmd)));
		if (!cmd)
			return;
		cmd->cmdID = RD_3DSPHERE_GRADIENT;
		cmd->center[0] = center.X;
		cmd->center[1] = center.Y;
		cmd->center[2] = center.Z;
		cmd->radius = radius;
		cmd->color1[0] = color1->R;
		cmd->color1[1] = color1->G;
		cmd->color1[2] = color1->B;
		cmd->color1[3] = color1->A;
		cmd->color2[0] = color2->R;
		cmd->color2[1] = color2->G;
		cmd->color2[2] = color2->B;
		cmd->color2[3] = color2->A;
	}

	void R_Draw3DSphereLightBeamGradient(const SDK::FVector& center, float radius, const SDK::FRotator& rotation, float beamHeight, float beamIntensity, SDK::FLinearColor* color1, SDK::FLinearColor* color2)
	{
		rd_3dsphere_lightbeam_gradient* cmd = static_cast<rd_3dsphere_lightbeam_gradient*>(R_GetCmdBuffer(sizeof(*cmd)));
		if (!cmd)
			return;
		cmd->cmdID = RD_3DSPHERE_LIGHTBEAM_GRADIENT;
		cmd->center[0] = center.X;
		cmd->center[1] = center.Y;
		cmd->center[2] = center.Z;
		cmd->radius = radius;
		cmd->rotation[0] = rotation.Pitch;
		cmd->rotation[1] = rotation.Yaw;
		cmd->rotation[2] = rotation.Roll;
		cmd->beamHeight = beamHeight;
		cmd->beamIntensity = beamIntensity;
		cmd->color1[0] = color1->R;
		cmd->color1[1] = color1->G;
		cmd->color1[2] = color1->B;
		cmd->color1[3] = color1->A;
		cmd->color2[0] = color2->R;
		cmd->color2[1] = color2->G;
		cmd->color2[2] = color2->B;
		cmd->color2[3] = color2->A;
	}

	void R_DrawCircleGradient(float x, float y, float radius, SDK::FLinearColor* color1, SDK::FLinearColor* color2)
	{
		rd_circle_gradient* cmd = static_cast<rd_circle_gradient*>(R_GetCmdBuffer(sizeof(*cmd)));
		if (!cmd)
			return;
		cmd->cmdID = RD_CIRCLE_GRADIENT;
		cmd->x = x;
		cmd->y = y;
		cmd->radius = radius;
		cmd->color1[0] = color1->R;
		cmd->color1[1] = color1->G;
		cmd->color1[2] = color1->B;
		cmd->color1[3] = color1->A;
		cmd->color2[0] = color2->R;
		cmd->color2[1] = color2->G;
		cmd->color2[2] = color2->B;
		cmd->color2[3] = color2->A;
	}

	void R_DrawRectGradient(float x, float y, float w, float h, SDK::FLinearColor* color1, SDK::FLinearColor* color2)
	{
		rd_rect_gradient* cmd = static_cast<rd_rect_gradient*>(R_GetCmdBuffer(sizeof(*cmd)));
		if (!cmd)
			return;
		cmd->cmdID = RD_RECT_GRADIENT;
		cmd->x = x;
		cmd->y = y;
		cmd->w = w;
		cmd->h = h;
		cmd->color1[0] = color1->R;
		cmd->color1[1] = color1->G;
		cmd->color1[2] = color1->B;
		cmd->color1[3] = color1->A;
		cmd->color2[0] = color2->R;
		cmd->color2[1] = color2->G;
		cmd->color2[2] = color2->B;
		cmd->color2[3] = color2->A;
	}

	void R_DrawLineGradient(float x, float y, float x2, float y2, SDK::FLinearColor* color1, SDK::FLinearColor* color2)
	{
		rd_line_gradient* cmd = (rd_line_gradient*)R_GetCmdBuffer(sizeof(rd_line_gradient));
		if (!cmd)
			return;
		cmd->cmdID = RD_LINE_GRADIENT;
		cmd->x = x;
		cmd->y = y;
		cmd->x2 = x2;
		cmd->y2 = y2;
		cmd->color1[0] = color1->R;
		cmd->color1[1] = color1->G;
		cmd->color1[2] = color1->B;
		cmd->color1[3] = color1->A;
		cmd->color2[0] = color2->R;
		cmd->color2[1] = color2->G;
		cmd->color2[2] = color2->B;
		cmd->color2[3] = color2->A;
	}

	void R_DrawTextGradient(float x, float y, std::string text, SDK::FLinearColor* color1, SDK::FLinearColor* color2, bool background)
	{
		rd_text_gradient* cmd = (rd_text_gradient*)R_GetCmdBuffer(sizeof(rd_text_gradient));
		if (!cmd)
			return;
		cmd->cmdID = RD_TEXT_GRADIENT;
		strcpy_s(cmd->txt, text.c_str());
		cmd->x = x;
		cmd->y = y;
		cmd->color1[0] = color1->R;
		cmd->color1[1] = color1->G;
		cmd->color1[2] = color1->B;
		cmd->color1[3] = color1->A;
		cmd->color2[0] = color2->R;
		cmd->color2[1] = color2->G;
		cmd->color2[2] = color2->B;
		cmd->color2[3] = color2->A;
		cmd->bg = background;
	}

	float GetSharedGradientTime()
	{
		static float sharedGradientTime = 0.0f;
		sharedGradientTime += 0.016f;    
		return sharedGradientTime;
	}

	void R_End()
	{
		rd_end* cmd;
		cmd = static_cast<rd_end*>(R_GetCmdBuffer(sizeof(*cmd)));
		if (!cmd)
			return;
		cmd->cmdID = RD_END;
		Commands.used = 0;
	}

	SDK::FLinearColor GetCyclingGradientColor(SDK::FLinearColor* color1, SDK::FLinearColor* color2, float time)
	{
		float t = (sinf(time * CFG::cfg_ESP_GradientSpeed) + 1.0f) * 0.5f;    
		return GetGradientColor(color1, color2, t);
	}
	SDK::FLinearColor GetFlowingRainbowColor(float position, float time)
	{
		float hue = fmodf(position * 3.6f + time * CFG::cfg_ESP_GradientSpeed, 6.0f);
		SDK::FLinearColor result;
		result.A = 1.0f;

		if (hue < 1.0f) {
			result.R = 1.0f;
			result.G = hue;
			result.B = 0.0f;
		}
		else if (hue < 2.0f) {
			result.R = 2.0f - hue;
			result.G = 1.0f;
			result.B = 0.0f;
		}
		else if (hue < 3.0f) {
			result.R = 0.0f;
			result.G = 1.0f;
			result.B = hue - 2.0f;
		}
		else if (hue < 4.0f) {
			result.R = 0.0f;
			result.G = 4.0f - hue;
			result.B = 1.0f;
		}
		else if (hue < 5.0f) {
			result.R = hue - 4.0f;
			result.G = 0.0f;
			result.B = 1.0f;
		}
		else {
			result.R = 1.0f;
			result.G = 0.0f;
			result.B = 6.0f - hue;
		}

		return result;
	}
}