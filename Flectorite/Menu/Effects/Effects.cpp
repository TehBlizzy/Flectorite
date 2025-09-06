#include "Effects.h"
#include "../Config.h"
#include <cmath>
#include <algorithm>

namespace Effects
{
    EffectsManager g_EffectsManager;

    EffectsManager::EffectsManager()
        : m_generator(std::random_device{}())
        , m_distribution(0.0f, 1.0f)
        , m_enableParticles(true)
        , m_enableRipples(true)
        , m_enableGlow(true)
        , m_enableSparkles(true)
        , m_particleCount(25)         
        , m_particleSpeed(30.0f)
        , m_particleSize(4.0f)
        , m_glowIntensity(0.5f)     
        , m_particleStyle(PARTICLE_SQUARE)
        , m_rippleSpeed(1.0f)
        , m_rippleMaxRadius(80.0f)     
        , m_rippleLifetime(1.2f)     
        , m_rippleSparkleCount(6)     
        , m_rippleSparkleBrightness(0.8f)     
        , m_rippleSparkleSize(1.5f)     
        , m_rippleSparkleRotationSpeed(1.0f)
        , m_enableGalaxy(true)
        , m_galaxyRotationSpeed(1.5f)      
        , m_galaxyArmCount(3.0f)
        , m_galaxyRadius(300.0f)     
        , m_galaxyTime(0.0f)
        , m_galaxySparkleCount(4)     
        , m_galaxySparkleBrightness(1.0f)     
        , m_galaxySparkleSize(2.5f)     
        , m_particlePulseIntensity(0.08f)     
        , m_particleDriftStrength(1.5f)     
        , m_galaxyCenterPulseIntensity(0.1f)     
        , m_galaxyTwinkleSpeed(2.5f)     
        , m_galaxyPositionX(0.0f)
        , m_galaxyPositionY(0.0f)
        , m_useCustomCursor(false)
        , m_cursorStyle(CURSOR_ARROW)
        , m_cursorSize(1.0f)
        , m_cursorGlowIntensity(0.5f)
        , m_enableCursorTrail(false)
        , m_cursorTrailLength(10.0f)
        , m_cursorSparkleCount(5)
        , m_cursorSparkleSize(2.5f)
        , m_lastCursorPos(0, 0)
        , m_cursorGalaxyArmCount(3.0f)
        , m_cursorGalaxyRadius(25.0f)
        , m_cursorGalaxyRotationSpeed(0.5f)
        , m_cursorGalaxyCenterColor(0.9f, 0.5f, 0.9f, 1.0f)
        , m_cursorGalaxyArmsColor(0.7f, 0.3f, 0.8f, 1.0f)
        , m_cursorGalaxySparklesColor(1.0f, 0.8f, 1.0f, 1.0f)
        , m_cursorEnableGalaxySparkles(true)
        , m_cursorGalaxySparkleCount(8)
        , m_cursorGalaxySparkleBrightness(0.8f)
        , m_cursorGalaxySparkleSize(2.0f)
        , m_cursorGalaxyCenterPulseIntensity(0.1f)
        , m_cursorGalaxyTwinkleSpeed(2.5f)
        , m_cursorGalaxyTime(0.0f)
    {
    }

    EffectsManager::~EffectsManager()
    {
        Shutdown();
    }

    void EffectsManager::Initialize()
    {
        m_particles.clear();
        m_ripples.clear();
        m_particles.resize(m_particleCount);
        m_ripples.reserve(20);     
    }

    void EffectsManager::Update(float deltaTime)
    {
        m_galaxyTime += deltaTime * m_galaxyRotationSpeed;
        m_cursorGalaxyTime += deltaTime * m_cursorGalaxyRotationSpeed;
        UpdateParticles(deltaTime, m_windowPos, m_windowSize);
        UpdateRipples(deltaTime);

        if (m_enableGalaxy)
        {
        }
    }

    void EffectsManager::Render(ImDrawList* drawList, const ImVec2& windowPos, const ImVec2& windowSize)
    {
        if (!drawList || windowSize.x <= 0 || windowSize.y <= 0)
            return;

        m_windowPos = windowPos;
        m_windowSize = windowSize;

        if (m_enableGalaxy)
        {
            RenderGalaxy(drawList, windowPos, windowSize);
        }

        if (m_enableParticles)
        {
            RenderParticles(drawList, windowPos, windowSize);
        }

        if (m_enableRipples)
        {
            RenderRipples(drawList);
        }
    }

    void EffectsManager::Shutdown()
    {
        m_particles.clear();
        m_ripples.clear();
    }

    void EffectsManager::InitializeParticles(const ImVec2& windowPos, const ImVec2& windowSize)
    {
        if (m_particles.size() != m_particleCount)
        {
            m_particles.resize(m_particleCount);
        }

        for (auto& particle : m_particles)
        {
            if (!particle.active)
            {
                particle.position.x = windowPos.x + m_distribution(m_generator) * windowSize.x;
                particle.position.y = windowPos.y + windowSize.y + 20;

                float speedVariation = 0.8f + m_distribution(m_generator) * 0.4f;     
                particle.velocity = ImVec2(
                    (m_distribution(m_generator) - 0.5f) * m_particleSpeed * speedVariation * 0.8f,    
                    -abs(m_distribution(m_generator) * m_particleSpeed * speedVariation * 1.2f)    
                );
                particle.rotationSpeed = (m_distribution(m_generator) - 0.5f) * 3.0f;    
                particle.size = 3.0f + m_distribution(m_generator) * (m_particleSize - 3.0f);
                particle.maxLifetime = 2.5f + m_distribution(m_generator) * 2.5f;    
                particle.alpha = 0.8f + m_distribution(m_generator) * 0.2f;    

                float brightnessVariation = -0.3f + m_distribution(m_generator) * 0.6f;    
                particle.brightness = 1.0f + brightnessVariation;
                particle.pulseOffset = m_distribution(m_generator) * 2.0f * IM_PI;    
                particle.rotation = m_distribution(m_generator) * 2.0f * IM_PI;
                particle.lifetime = 0.0f;
                particle.active = true;
            }
        }
    }

    void EffectsManager::UpdateParticles(float deltaTime, const ImVec2& windowPos, const ImVec2& windowSize)
    {
        for (auto& particle : m_particles)
        {
            if (particle.active)
            {
                particle.velocity.x *= 0.98f;    
                particle.velocity.y += 8.0f * deltaTime;    

                float driftStrength = sinf(particle.lifetime * 0.5f + particle.rotation) * m_particleDriftStrength;
                particle.velocity.x += driftStrength * deltaTime;
                particle.position.x += particle.velocity.x * deltaTime;
                particle.position.y += particle.velocity.y * deltaTime;
                particle.rotation += particle.rotationSpeed * deltaTime;
                particle.lifetime += deltaTime;

                if (particle.position.y < windowPos.y - 100 ||
                    particle.position.x < windowPos.x - 50 ||
                    particle.position.x > windowPos.x + windowSize.x + 50 ||
                    particle.lifetime >= particle.maxLifetime)
                {
                    particle.active = false;
                }
            }
        }

        InitializeParticles(windowPos, windowSize);
    }

    void EffectsManager::RenderParticles(ImDrawList* drawList, const ImVec2& windowPos, const ImVec2& windowSize)
    {
        for (const auto& particle : m_particles)
        {
            if (particle.active)
            {
                if (particle.position.x >= windowPos.x - 50 && particle.position.x <= windowPos.x + windowSize.x + 50 &&
                    particle.position.y >= windowPos.y - 50 && particle.position.y <= windowPos.y + windowSize.y + 50)
                {
                    float lifetimeProgress = particle.lifetime / particle.maxLifetime;
                    float smoothFade = 1.0f - (3.0f * lifetimeProgress * lifetimeProgress - 2.0f * lifetimeProgress * lifetimeProgress * lifetimeProgress);  
                    float finalAlpha = particle.alpha * smoothFade * 0.95f;      

                    float sizeMultiplier = 0.3f + smoothFade * 0.7f;       
                    float pulseEffect = 1.0f + sinf(particle.lifetime * 3.0f + particle.pulseOffset) * m_particlePulseIntensity;   
                    sizeMultiplier *= pulseEffect;

                    ImColor particleColor = ImColor(m_colors.particleColor.x, m_colors.particleColor.y, m_colors.particleColor.z, finalAlpha);
                    ImColor glowColor = ImColor(m_colors.particleColor.x, m_colors.particleColor.y, m_colors.particleColor.z, finalAlpha * 0.8f);

                    if (m_enableGlow)
                    {
                        for (int i = 1; i <= 3; i++)        
                        {
                            float glowExpansion = i * 2.0f;    
                            float glowAlpha = (m_glowIntensity * finalAlpha * 0.8f) / (i * i);      
                            ImColor layerGlow = ImColor(m_colors.glow.x, m_colors.glow.y, m_colors.glow.z, glowAlpha);
                            Particle glowParticle = particle;
                            glowParticle.size *= sizeMultiplier;
                            RenderParticleGlow(drawList, glowParticle, layerGlow, m_particleStyle, glowExpansion);
                        }
                    }

                    Particle sizedParticle = particle;
                    sizedParticle.size *= sizeMultiplier;
                    RenderParticleShape(drawList, sizedParticle, particleColor, m_particleStyle);
                }
            }
        }
    }

    void EffectsManager::RenderGalaxy(ImDrawList* drawList, const ImVec2& windowPos, const ImVec2& windowSize)
    {
        if (!m_enableGalaxy || !drawList) return;

        ImVec2 center = ImVec2(windowPos.x + windowSize.x * 0.5f + m_galaxyPositionX, windowPos.y + windowSize.y * 0.5f + m_galaxyPositionY);


        float centerPulse = 1.0f + sinf(m_galaxyTime * 0.8f) * m_galaxyCenterPulseIntensity;    
        for (int layer = 0; layer < 6; layer++)
        {
            float centerRadius = (8.0f + layer * 6.0f) * centerPulse;
            float layerAlpha = 0.08f * (1.0f - layer * 0.15f) * centerPulse;    
            ImU32 layerColor = IM_COL32((int)(m_colors.galaxyCenter.x * 255), (int)(m_colors.galaxyCenter.y * 255), (int)(m_colors.galaxyCenter.z * 255), (int)(layerAlpha * 255));
            drawList->AddCircleFilled(center, centerRadius, layerColor, 24);
        }

        for (int arm = 0; arm < (int)m_galaxyArmCount; arm++)
        {
            float armOffset = (arm * 2.0f * IM_PI / m_galaxyArmCount) + m_galaxyTime;

            float armAlphaVariation = 0.7f + (arm % 3) * 0.2f + sinf(arm * 1.3f) * 0.3f;    

            for (float t = 0.0f; t < 5.0f * IM_PI; t += 0.12f)
            {
                float spiralRadius = (t / (5.0f * IM_PI)) * m_galaxyRadius;
                float angle = t * 0.5f + armOffset;   

                ImVec2 spiralPos = ImVec2(
                    center.x + cosf(angle) * spiralRadius,
                    center.y + sinf(angle) * spiralRadius
                );

                float distanceFactor = 1.0f - (spiralRadius / m_galaxyRadius);
                float centerBrightness = 1.0f + distanceFactor * 0.8f;   
                float armBrightness = 0.6f + 0.4f * sinf(t * 2.0f);    
                float edgeBrightness = 0.3f + distanceFactor * 0.4f;   

                float localVar = sinf(t * 12.0f + arm * 4.0f) * 0.5f + sinf(t * 20.0f) * 0.3f;
                float localBrightness = 1.0f + localVar;

                float totalBrightness = (centerBrightness + armBrightness + edgeBrightness) / 3.0f * armAlphaVariation * localBrightness;

                float edgeFade = 1.0f - (spiralRadius / m_galaxyRadius);
                edgeFade = edgeFade * edgeFade;     
                totalBrightness *= edgeFade;

                if (spiralPos.x >= windowPos.x && spiralPos.x <= windowPos.x + windowSize.x &&
                    spiralPos.y >= windowPos.y && spiralPos.y <= windowPos.y + windowSize.y &&
                    totalBrightness > 0.01f)
                {
                    float alpha = totalBrightness * 0.35f;     

                    for (int layer = 0; layer < 4; layer++)
                    {
                        float layerRadius = 2.0f + layer * 1.2f;   
                        float layerAlpha = alpha * (1.0f - layer * 0.3f);   

                        float distanceFromCenter = sqrtf((spiralPos.x - center.x) * (spiralPos.x - center.x) +
                            (spiralPos.y - center.y) * (spiralPos.y - center.y));
                        float maxDistance = m_galaxyRadius * 0.5f;       
                        float colorBlend = fminf(distanceFromCenter / maxDistance, 1.0f);       

                        float r = m_colors.galaxyCenter.x * (1.0f - colorBlend) + m_colors.galaxyArms.x * colorBlend;
                        float g = m_colors.galaxyCenter.y * (1.0f - colorBlend) + m_colors.galaxyArms.y * colorBlend;
                        float b = m_colors.galaxyCenter.z * (1.0f - colorBlend) + m_colors.galaxyArms.z * colorBlend;

                        ImU32 layerColor = IM_COL32((int)(r * 255), (int)(g * 255), (int)(b * 255), (int)(layerAlpha * 255));
                        drawList->AddCircleFilled(spiralPos, layerRadius, layerColor, 12);
                    }

                    if (totalBrightness > 1.8f)      
                    {
                        float twinkleTime = m_galaxyTime * 4.0f + t * 2.0f + arm;
                        float twinkleIntensity = sinf(twinkleTime) * 0.5f + 0.5f;    

                        if (twinkleIntensity > 0.6f)      
                        {
                            float starSize = 1.5f + twinkleIntensity * 2.0f;     
                            float starAlpha = (twinkleIntensity - 0.6f) * 2.5f;   

                            ImColor starColor = ImColor(
                                m_colors.galaxySparkles.x,
                                m_colors.galaxySparkles.y,
                                m_colors.galaxySparkles.z,
                                starAlpha * 0.8f
                            );

                            drawList->AddCircleFilled(spiralPos, starSize, starColor, 8);
                        }
                    }
                }

                if (m_enableSparkles)
                {
                    for (int sparkle = 0; sparkle < m_galaxySparkleCount; sparkle++)
                    {
                        float sparkleT = t + (sparkle * 0.3f / m_galaxySparkleCount) - 0.15f;    
                        float sparkleRadius = 20.0f + sparkleT * m_galaxyRadius;

                        float sparkleArmAngle = sparkleT * 5.0f * IM_PI + armOffset;

                        ImVec2 sparklePos = ImVec2(
                            center.x + cosf(sparkleArmAngle) * sparkleRadius,
                            center.y + sinf(sparkleArmAngle) * sparkleRadius
                        );

                        float twinkle = sinf(sparkleT * 8.0f + m_galaxyTime * m_galaxyTwinkleSpeed + arm * 2.0f) * 0.5f + 0.5f;
                        if (twinkle > 0.3f && sparkleRadius < m_galaxyRadius)        
                        {
                            float sparkleDistanceFactor = 1.0f - (sparkleRadius / m_galaxyRadius);
                            float sparkleBrightness = 0.3f + sparkleDistanceFactor * 0.7f;    

                            float sparkleAlpha = sparkleBrightness * m_galaxySparkleBrightness * 0.8f;       

                            ImColor sparkleColor = ImColor(m_colors.galaxySparkles.x, m_colors.galaxySparkles.y, m_colors.galaxySparkles.z, sparkleAlpha);

                            float sparkleSize = m_galaxySparkleSize * (0.8f + sparkleDistanceFactor * 0.4f);     
                            for (int point = 0; point < 5; point++)
                            {
                                float sparkleRotation = sparkleArmAngle + IM_PI * 0.5f;     
                                float outerAngle = (point * 2.0f * IM_PI / 5.0f) + sparkleRotation;
                                float innerAngle1 = ((point + 0.5f) * 2.0f * IM_PI / 5.0f) + sparkleRotation;
                                float innerAngle2 = ((point - 0.5f) * 2.0f * IM_PI / 5.0f) + sparkleRotation;

                                ImVec2 outerPoint = ImVec2(
                                    sparklePos.x + cosf(outerAngle) * sparkleSize,
                                    sparklePos.y + sinf(outerAngle) * sparkleSize
                                );
                                ImVec2 innerPoint1 = ImVec2(
                                    sparklePos.x + cosf(innerAngle1) * sparkleSize * 0.4f,
                                    sparklePos.y + sinf(innerAngle1) * sparkleSize * 0.4f
                                );
                                ImVec2 innerPoint2 = ImVec2(
                                    sparklePos.x + cosf(innerAngle2) * sparkleSize * 0.4f,
                                    sparklePos.y + sinf(innerAngle2) * sparkleSize * 0.4f
                                );

                                ImVec2 triangle[3] = { outerPoint, innerPoint1, innerPoint2 };
                                drawList->AddConvexPolyFilled(triangle, 3, sparkleColor);
                            }
                        }
                    }
                }    
            }
        }
    }

    void EffectsManager::CreateRipple(const ImVec2& position, float maxRadius)
    {
        if (m_ripples.empty())
            m_ripples.resize(20);

        for (auto& ripple : m_ripples)
        {
            if (!ripple.active)
            {
                ripple.center = position;
                ripple.radius = 0.0f;
                ripple.maxRadius = maxRadius;
                ripple.alpha = 1.0f;
                ripple.lifetime = 0.0f;
                ripple.maxLifetime = m_rippleLifetime;
                ripple.active = true;
                ripple.color = IM_COL32((int)(m_colors.rippleColor.x * 255), (int)(m_colors.rippleColor.y * 255), (int)(m_colors.rippleColor.z * 255), 200);
                ripple.timeOffset = m_distribution(m_generator) * 2.0f * IM_PI;
                ripple.sparkleCount = m_rippleSparkleCount;
                ripple.sparkleBrightness = m_rippleSparkleBrightness;
                ripple.sparkleSize = m_rippleSparkleSize;
                ripple.sparkleRotationSpeed = m_rippleSparkleRotationSpeed;

                ripple.sparkleLifetimes.resize(ripple.sparkleCount);
                ripple.sparkleMaxLifetimes.resize(ripple.sparkleCount);
                ripple.sparkleSizes.resize(ripple.sparkleCount);
                ripple.sparkleTargetSizes.resize(ripple.sparkleCount);

                for (int i = 0; i < ripple.sparkleCount; i++)
                {
                    ripple.sparkleLifetimes[i] = 0.0f;
                    ripple.sparkleMaxLifetimes[i] = 1.5f + m_distribution(m_generator) * 1.0f;
                    ripple.sparkleSizes[i] = ripple.sparkleSize * (0.6f + m_distribution(m_generator) * 0.8f);
                    ripple.sparkleTargetSizes[i] = ripple.sparkleSize * (0.8f + m_distribution(m_generator) * 0.6f);
                }
                break;
            }
        }
    }

    void EffectsManager::UpdateRipples(float deltaTime)
    {
        for (auto& ripple : m_ripples)
        {
            if (ripple.active)
            {
                ripple.lifetime += deltaTime;
                float progress = ripple.lifetime / ripple.maxLifetime;

                if (progress >= 1.0f)
                {
                    ripple.active = false;
                }
                else
                {
                    ripple.radius = ripple.maxRadius * progress * m_rippleSpeed;
                    ripple.alpha = 0.8f * (1.0f - progress);

                    for (int i = 0; i < ripple.sparkleCount && i < ripple.sparkleLifetimes.size(); i++)
                    {
                        ripple.sparkleLifetimes[i] += deltaTime;

                        float sparkleProgress = ripple.sparkleLifetimes[i] / ripple.sparkleMaxLifetimes[i];
                        if (sparkleProgress < 1.0f)
                        {
                            float currentSize = ripple.sparkleSizes[i];
                            float targetSize = ripple.sparkleTargetSizes[i];
                            ripple.sparkleSizes[i] = currentSize + (targetSize - currentSize) * sparkleProgress * 0.5f;
                        }
                    }
                }
            }
        }
    }

    void EffectsManager::RenderRipples(ImDrawList* drawList)
    {
        for (const auto& ripple : m_ripples)
        {
            if (ripple.active)
            {
                float r = m_colors.rippleColor.x;
                float g = m_colors.rippleColor.y;
                float b = m_colors.rippleColor.z;

                for (int layer = 0; layer < 3; layer++)
                {
                    float layerRadius = ripple.radius - (layer * 2.0f);
                    if (layerRadius > 0)
                    {
                        float layerAlpha = ripple.alpha * (1.0f - layer * 0.05f);      

                        ImColor layerColor = ImColor(r, g, b, layerAlpha);

                        float thickness = 2.0f - (layer * 0.3f);

                        int segments = layer == 0 ? 48 : 32;       
                        drawList->AddCircle(ripple.center, layerRadius, layerColor, segments, thickness);
                    }
                }

                if (m_enableSparkles)
                {
                    RenderRippleSparkles(drawList, ripple);
                }
            }
        }
    }

    void EffectsManager::RenderRippleSparkles(ImDrawList* drawList, const Ripple& ripple)
    {
        float r = m_colors.rippleColor.x;
        float g = m_colors.rippleColor.y;
        float b = m_colors.rippleColor.z;

        float sparkleAlpha = ripple.alpha * 1.0f;      

        for (int i = 0; i < m_rippleSparkleCount; i++)
        {
            float baseAngle = (i * 2.0f * IM_PI / m_rippleSparkleCount);
            float rotationOffset = ripple.timeOffset * m_rippleSparkleRotationSpeed;
            float angle = baseAngle + rotationOffset;

            float radiusVariation = sinf(baseAngle * 3.0f + ripple.timeOffset) * 3.0f;
            float sparkleRadius = ripple.radius + radiusVariation;

            ImVec2 sparklePos = ImVec2(
                ripple.center.x + cosf(angle) * sparkleRadius,
                ripple.center.y + sinf(angle) * sparkleRadius
            );

            float brightness = 0.8f + 0.2f * sinf(angle * 1.8f + ripple.timeOffset * 1.5f);   
            float individualAlpha = sparkleAlpha;

            ImColor sparkleColor = ImColor(r, g, b, individualAlpha);

            float individualSize = (i < ripple.sparkleSizes.size()) ? ripple.sparkleSizes[i] : ripple.sparkleSize;
            float individualLifetime = (i < ripple.sparkleLifetimes.size()) ? ripple.sparkleLifetimes[i] : 0.0f;
            float individualMaxLifetime = (i < ripple.sparkleMaxLifetimes.size()) ? ripple.sparkleMaxLifetimes[i] : 2.0f;

            float sparkleProgress = individualLifetime / individualMaxLifetime;
            float sparkleLifetimeAlpha = (sparkleProgress < 1.0f) ? (1.0f - sparkleProgress) : 0.0f;
            float finalSparkleAlpha = individualAlpha * sparkleLifetimeAlpha;

            if (sparkleProgress >= 1.0f || finalSparkleAlpha <= 0.01f) continue;

            for (int layer = 0; layer < 2; layer++)
            {
                float layerSize = individualSize * (1.0f + layer * 0.3f);
                float layerAlpha = finalSparkleAlpha * (1.0f - layer * 0.2f);      
                ImColor layerColor = ImColor(r, g, b, layerAlpha);

                for (int point = 0; point < 5; point++)
                {
                    float outerAngle = (point * 2.0f * IM_PI / 5.0f) + angle;
                    float innerAngle1 = ((point + 0.5f) * 2.0f * IM_PI / 5.0f) + angle;
                    float innerAngle2 = ((point - 0.5f) * 2.0f * IM_PI / 5.0f) + angle;

                    ImVec2 outerPoint = ImVec2(
                        sparklePos.x + cosf(outerAngle) * layerSize,
                        sparklePos.y + sinf(outerAngle) * layerSize
                    );
                    ImVec2 innerPoint1 = ImVec2(
                        sparklePos.x + cosf(innerAngle1) * layerSize * 0.4f,
                        sparklePos.y + sinf(innerAngle1) * layerSize * 0.4f
                    );
                    ImVec2 innerPoint2 = ImVec2(
                        sparklePos.x + cosf(innerAngle2) * layerSize * 0.4f,
                        sparklePos.y + sinf(innerAngle2) * layerSize * 0.4f
                    );

                    ImVec2 triangle[3] = { outerPoint, innerPoint1, innerPoint2 };
                    drawList->AddConvexPolyFilled(triangle, 3, layerColor);
                }
            }
        }
    }

    void EffectsManager::RenderParticleGlow(ImDrawList* drawList, const Particle& particle, ImColor glowColor, ParticleStyle style, float glowExpansion)
    {
        if (!m_enableGlow) return;

        for (int layer = 2; layer >= 1; layer--)        
        {
            float layerExpansion = glowExpansion * layer * 0.25f;   
            float layerAlpha = glowColor.Value.w * (0.4f / (layer * layer));   
            ImColor layerColor = ImColor(glowColor.Value.x, glowColor.Value.y, glowColor.Value.z, layerAlpha);

            float halfSize = (particle.size + layerExpansion) * 0.5f;
            float cos_a = cosf(particle.rotation);
            float sin_a = sinf(particle.rotation);

            switch (style)
            {
            case PARTICLE_SQUARE:
            {
                ImVec2 corners[4] = {
                    ImVec2(-halfSize, -halfSize), ImVec2(halfSize, -halfSize),
                    ImVec2(halfSize, halfSize), ImVec2(-halfSize, halfSize)
                };

                for (int i = 0; i < 4; i++)
                {
                    float x = corners[i].x * cos_a - corners[i].y * sin_a;
                    float y = corners[i].x * sin_a + corners[i].y * cos_a;
                    corners[i] = ImVec2(particle.position.x + x, particle.position.y + y);
                }
                drawList->AddConvexPolyFilled(corners, 4, layerColor);
                break;
            }
            case PARTICLE_CIRCLE:
            {
                drawList->AddCircleFilled(particle.position, halfSize, layerColor, 16);
                break;
            }
            case PARTICLE_TRIANGLE:
            {
                ImVec2 corners[3] = {
                    ImVec2(0, -halfSize * 1.2f),
                    ImVec2(-halfSize * 0.9f, halfSize * 0.6f),
                    ImVec2(halfSize * 0.9f, halfSize * 0.6f)
                };

                for (int i = 0; i < 3; i++)
                {
                    float x = corners[i].x * cos_a - corners[i].y * sin_a;
                    float y = corners[i].x * sin_a + corners[i].y * cos_a;
                    corners[i] = ImVec2(particle.position.x + x, particle.position.y + y);
                }
                drawList->AddConvexPolyFilled(corners, 3, layerColor);
                break;
            }
            case PARTICLE_STAR:
            {
                for (int point = 0; point < 5; point++)
                {
                    float outerAngle = (point * 2.0f * IM_PI / 5.0f) + particle.rotation;
                    float innerAngle1 = ((point + 0.5f) * 2.0f * IM_PI / 5.0f) + particle.rotation;
                    float innerAngle2 = ((point - 0.5f) * 2.0f * IM_PI / 5.0f) + particle.rotation;

                    ImVec2 outerPoint = ImVec2(
                        particle.position.x + cosf(outerAngle) * halfSize,
                        particle.position.y + sinf(outerAngle) * halfSize
                    );
                    ImVec2 innerPoint1 = ImVec2(
                        particle.position.x + cosf(innerAngle1) * halfSize * 0.4f,
                        particle.position.y + sinf(innerAngle1) * halfSize * 0.4f
                    );
                    ImVec2 innerPoint2 = ImVec2(
                        particle.position.x + cosf(innerAngle2) * halfSize * 0.4f,
                        particle.position.y + sinf(innerAngle2) * halfSize * 0.4f
                    );

                    ImVec2 triangle[3] = { outerPoint, innerPoint1, innerPoint2 };
                    drawList->AddConvexPolyFilled(triangle, 3, layerColor);
                }
                break;
            }
            case PARTICLE_DIAMOND:
            {
                ImVec2 corners[4] = {
                    ImVec2(0, -halfSize), ImVec2(halfSize, 0),
                    ImVec2(0, halfSize), ImVec2(-halfSize, 0)
                };

                for (int i = 0; i < 4; i++)
                {
                    float x = corners[i].x * cos_a - corners[i].y * sin_a;
                    float y = corners[i].x * sin_a + corners[i].y * cos_a;
                    corners[i] = ImVec2(particle.position.x + x, particle.position.y + y);
                }
                drawList->AddConvexPolyFilled(corners, 4, layerColor);
                break;
            }
            }
        }
    }

    void EffectsManager::RenderParticleShape(ImDrawList* drawList, const Particle& particle, ImColor color, ParticleStyle style)
    {
        float halfSize = particle.size * 0.5f;
        float cos_a = cosf(particle.rotation);
        float sin_a = sinf(particle.rotation);

        switch (style)
        {
        case PARTICLE_SQUARE:
        {
            ImVec2 corners[4] = {
                ImVec2(-halfSize, -halfSize), ImVec2(halfSize, -halfSize),
                ImVec2(halfSize, halfSize), ImVec2(-halfSize, halfSize)
            };

            for (int i = 0; i < 4; i++)
            {
                float x = corners[i].x * cos_a - corners[i].y * sin_a;
                float y = corners[i].x * sin_a + corners[i].y * cos_a;
                corners[i] = ImVec2(particle.position.x + x, particle.position.y + y);
            }
            drawList->AddConvexPolyFilled(corners, 4, color);
            break;
        }
        case PARTICLE_CIRCLE:
        {
            drawList->AddCircleFilled(particle.position, halfSize, color, 16);
            break;
        }
        case PARTICLE_TRIANGLE:
        {
            ImVec2 corners[3] = {
                ImVec2(0, -halfSize * 1.2f),
                ImVec2(-halfSize * 0.9f, halfSize * 0.6f),
                ImVec2(halfSize * 0.9f, halfSize * 0.6f)
            };

            for (int i = 0; i < 3; i++)
            {
                float x = corners[i].x * cos_a - corners[i].y * sin_a;
                float y = corners[i].x * sin_a + corners[i].y * cos_a;
                corners[i] = ImVec2(particle.position.x + x, particle.position.y + y);
            }
            drawList->AddConvexPolyFilled(corners, 3, color);
            break;
        }
        case PARTICLE_STAR:
        {
            for (int point = 0; point < 5; point++)
            {
                float outerAngle = (point * 2.0f * IM_PI / 5.0f) + particle.rotation;
                float innerAngle1 = ((point + 0.5f) * 2.0f * IM_PI / 5.0f) + particle.rotation;
                float innerAngle2 = ((point - 0.5f) * 2.0f * IM_PI / 5.0f) + particle.rotation;

                ImVec2 outerPoint = ImVec2(
                    particle.position.x + cosf(outerAngle) * halfSize,
                    particle.position.y + sinf(outerAngle) * halfSize
                );
                ImVec2 innerPoint1 = ImVec2(
                    particle.position.x + cosf(innerAngle1) * halfSize * 0.4f,
                    particle.position.y + sinf(innerAngle1) * halfSize * 0.4f
                );
                ImVec2 innerPoint2 = ImVec2(
                    particle.position.x + cosf(innerAngle2) * halfSize * 0.4f,
                    particle.position.y + sinf(innerAngle2) * halfSize * 0.4f
                );

                ImVec2 triangle[3] = { outerPoint, innerPoint1, innerPoint2 };
                drawList->AddConvexPolyFilled(triangle, 3, color);
            }
            break;
        }
        case PARTICLE_DIAMOND:
        {
            ImVec2 corners[4] = {
                ImVec2(0, -halfSize * 1.3f), ImVec2(halfSize * 0.8f, 0),
                ImVec2(0, halfSize * 1.3f), ImVec2(-halfSize * 0.8f, 0)
            };

            for (int i = 0; i < 4; i++)
            {
                float x = corners[i].x * cos_a - corners[i].y * sin_a;
                float y = corners[i].x * sin_a + corners[i].y * cos_a;
                corners[i] = ImVec2(particle.position.x + x, particle.position.y + y);
            }
            drawList->AddConvexPolyFilled(corners, 4, color);
            break;
        }
        }
    }

    void EffectsManager::DrawGlow(ImDrawList* drawList, const ImVec2& min, const ImVec2& max, ImU32 color, float intensity, float rounding)
    {
        if (!m_enableGlow) return;

        for (int i = 1; i <= 4; i++)
        {
            float expansion = i * 2.0f;
            float alpha = (intensity * 0.4f) / (i + 1);
            ImU32 glowColor = (color & 0x00FFFFFF) | ((ImU32)(alpha * 255.0f) << 24);

            drawList->AddRectFilled(
                ImVec2(min.x - expansion, min.y - expansion),
                ImVec2(max.x + expansion, max.y + expansion),
                glowColor, rounding + expansion
            );
        }
    }

    void EffectsManager::DrawGlowText(ImDrawList* drawList, const ImVec2& pos, const char* text, ImColor textColor, ImColor glowColor, float intensity)
    {
        if (!m_enableGlow) return;

        for (int i = 2; i >= 1; i--)
        {
            float expansion = i * 1.0f;
            float alpha = (intensity * 0.5f) / (i + 1);
            ImU32 layerGlow = IM_COL32(
                (int)(glowColor.Value.x * 255),
                (int)(glowColor.Value.y * 255),
                (int)(glowColor.Value.z * 255),
                (int)(255 * alpha)
            );

            for (int dx = -1; dx <= 1; dx++)
            {
                for (int dy = -1; dy <= 1; dy++)
                {
                    if (dx == 0 && dy == 0) continue;
                    drawList->AddText(
                        ImVec2(pos.x + dx * expansion, pos.y + dy * expansion),
                        layerGlow, text
                    );
                }
            }
        }

        drawList->AddText(pos, textColor, text);
    }
    void EffectsManager::UpdateCursor(float deltaTime)
    {
        if (!m_useCustomCursor) return;

        ImVec2 currentPos = ImGui::GetMousePos();

        m_cursorGalaxyTime += deltaTime * m_cursorGalaxyRotationSpeed;

        if (m_enableCursorTrail)
        {
            if (ImLengthSqr(ImVec2(currentPos.x - m_lastCursorPos.x, currentPos.y - m_lastCursorPos.y)) > 4.0f)
            {
                CursorTrailPoint newPoint;
                newPoint.position = currentPos;
                newPoint.alpha = 1.0f;
                newPoint.lifetime = 0.0f;
                newPoint.maxLifetime = m_cursorTrailLength * 0.1f;

                m_cursorTrail.insert(m_cursorTrail.begin(), newPoint);

                if (m_cursorTrail.size() > (size_t)m_cursorTrailLength)
                {
                    m_cursorTrail.resize((size_t)m_cursorTrailLength);
                }
            }

            for (auto it = m_cursorTrail.begin(); it != m_cursorTrail.end();)
            {
                it->lifetime += deltaTime;
                it->alpha = 1.0f - (it->lifetime / it->maxLifetime);

                if (it->lifetime >= it->maxLifetime)
                {
                    it = m_cursorTrail.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

        m_lastCursorPos = currentPos;
    }

    void DrawStar(ImDrawList* drawList, const ImVec2& center, float size, float rotation, ImU32 color)
    {
        float halfSize = size * 0.5f;
        for (int point = 0; point < 5; point++)
        {
            float outerAngle = (point * 2.0f * IM_PI / 5.0f) + rotation;
            float innerAngle1 = ((point + 0.5f) * 2.0f * IM_PI / 5.0f) + rotation;
            float innerAngle2 = ((point - 0.5f) * 2.0f * IM_PI / 5.0f) + rotation;

            ImVec2 outerPoint = ImVec2(
                center.x + cosf(outerAngle) * halfSize,
                center.y + sinf(outerAngle) * halfSize
            );
            ImVec2 innerPoint1 = ImVec2(
                center.x + cosf(innerAngle1) * halfSize * 0.4f,
                center.y + sinf(innerAngle1) * halfSize * 0.4f
            );
            ImVec2 innerPoint2 = ImVec2(
                center.x + cosf(innerAngle2) * halfSize * 0.4f,
                center.y + sinf(innerAngle2) * halfSize * 0.4f
            );

            ImVec2 triangle[3] = { outerPoint, innerPoint1, innerPoint2 };
            drawList->AddConvexPolyFilled(triangle, 3, color);
        }
    }

    void EffectsManager::RenderCursorGalaxy(ImDrawList* drawList, const ImVec2& center, float scale)
    {
        if (!drawList) return;

        float centerPulse = 1.0f + sinf(m_cursorGalaxyTime * 0.8f) * m_cursorGalaxyCenterPulseIntensity;
        for (int layer = 0; layer < 6; layer++)
        {
            float centerRadius = (2.0f + layer * 1.5f) * centerPulse * scale;
            float layerAlpha = 0.08f * (1.0f - layer * 0.15f) * centerPulse;
            ImU32 layerColor = IM_COL32(
                (int)(m_cursorGalaxyCenterColor.x * 255),
                (int)(m_cursorGalaxyCenterColor.y * 255),
                (int)(m_cursorGalaxyCenterColor.z * 255),
                (int)(layerAlpha * 255)
            );
            drawList->AddCircleFilled(center, centerRadius, layerColor, 24);
        }

        for (int arm = 0; arm < (int)m_cursorGalaxyArmCount; arm++)
        {
            float armOffset = (arm * 2.0f * IM_PI / m_cursorGalaxyArmCount) + m_cursorGalaxyTime;
            float armAlphaVariation = 0.7f + (arm % 3) * 0.2f + sinf(arm * 1.3f) * 0.3f;

            for (float t = 0.0f; t < 5.0f * IM_PI; t += 0.15f)
            {
                float spiralRadius = (t / (5.0f * IM_PI)) * m_cursorGalaxyRadius * scale;
                float angle = t * 0.5f + armOffset;

                ImVec2 spiralPos = ImVec2(
                    center.x + cosf(angle) * spiralRadius,
                    center.y + sinf(angle) * spiralRadius
                );

                float distanceFactor = 1.0f - (spiralRadius / (m_cursorGalaxyRadius * scale));
                float centerBrightness = 1.0f + distanceFactor * 0.8f;
                float armBrightness = 0.6f + 0.4f * sinf(t * 2.0f);
                float edgeBrightness = 0.3f + distanceFactor * 0.4f;

                float localVar = sinf(t * 12.0f + arm * 4.0f) * 0.5f + sinf(t * 20.0f) * 0.3f;
                float localBrightness = 1.0f + localVar;

                float totalBrightness = (centerBrightness + armBrightness + edgeBrightness) / 3.0f * armAlphaVariation * localBrightness;

                float edgeFade = 1.0f - (spiralRadius / (m_cursorGalaxyRadius * scale));
                edgeFade = edgeFade * edgeFade;
                totalBrightness *= edgeFade;

                if (totalBrightness > 0.01f)
                {
                    float alpha = totalBrightness * 0.35f;

                    for (int layer = 0; layer < 4; layer++)
                    {
                        float layerRadius = (2.0f + layer * 1.2f) * scale;
                        float layerAlpha = alpha * (1.0f - layer * 0.3f);

                        float distanceFromCenter = sqrtf((spiralPos.x - center.x) * (spiralPos.x - center.x) +
                            (spiralPos.y - center.y) * (spiralPos.y - center.y));
                        float maxDistance = m_cursorGalaxyRadius * scale * 0.5f;
                        float colorBlend = fminf(distanceFromCenter / maxDistance, 1.0f);

                        float r = m_cursorGalaxyCenterColor.x * (1.0f - colorBlend) + m_cursorGalaxyArmsColor.x * colorBlend;
                        float g = m_cursorGalaxyCenterColor.y * (1.0f - colorBlend) + m_cursorGalaxyArmsColor.y * colorBlend;
                        float b = m_cursorGalaxyCenterColor.z * (1.0f - colorBlend) + m_cursorGalaxyArmsColor.z * colorBlend;

                        ImU32 layerColor = IM_COL32((int)(r * 255), (int)(g * 255), (int)(b * 255), (int)(layerAlpha * 255));
                        drawList->AddCircleFilled(spiralPos, layerRadius, layerColor, 12);
                    }

                    if (totalBrightness > 1.8f)
                    {
                        float twinkleTime = m_cursorGalaxyTime * 4.0f + t * 2.0f + arm;
                        float twinkleIntensity = sinf(twinkleTime) * 0.5f + 0.5f;

                        if (twinkleIntensity > 0.6f)
                        {
                            float starSize = (0.4f + twinkleIntensity * 0.5f) * scale;
                            float starAlpha = (twinkleIntensity - 0.6f) * 2.5f;

                            ImColor starColor = ImColor(
                                m_cursorGalaxySparklesColor.x,
                                m_cursorGalaxySparklesColor.y,
                                m_cursorGalaxySparklesColor.z,
                                starAlpha * 0.8f
                            );

                            drawList->AddCircleFilled(spiralPos, starSize, starColor, 8);
                        }
                    }
                }

                if (m_cursorEnableGalaxySparkles)
                {
                    for (int sparkle = 0; sparkle < m_cursorGalaxySparkleCount; sparkle++)
                    {
                        float sparkleT = t + (sparkle * 0.3f / m_cursorGalaxySparkleCount) - 0.15f;
                        float sparkleRadius = 5.0f + sparkleT * m_cursorGalaxyRadius * scale;

                        float sparkleArmAngle = sparkleT * 5.0f * IM_PI + armOffset;

                        ImVec2 sparklePos = ImVec2(
                            center.x + cosf(sparkleArmAngle) * sparkleRadius,
                            center.y + sinf(sparkleArmAngle) * sparkleRadius
                        );

                        float twinkle = sinf(sparkleT * 8.0f + m_cursorGalaxyTime * m_cursorGalaxyTwinkleSpeed + arm * 2.0f) * 0.5f + 0.5f;
                        if (twinkle > 0.3f && sparkleRadius < m_cursorGalaxyRadius * scale)
                        {
                            float sparkleDistanceFactor = 1.0f - (sparkleRadius / (m_cursorGalaxyRadius * scale));
                            float sparkleBrightness = 0.3f + sparkleDistanceFactor * 0.7f;
                            float sparkleAlpha = sparkleBrightness * m_cursorGalaxySparkleBrightness * 0.8f;

                            ImColor sparkleColor = ImColor(
                                m_cursorGalaxySparklesColor.x,
                                m_cursorGalaxySparklesColor.y,
                                m_cursorGalaxySparklesColor.z,
                                sparkleAlpha
                            );

                            float sparkleSize = m_cursorGalaxySparkleSize * (0.8f + sparkleDistanceFactor * 0.4f) * scale;
                            for (int point = 0; point < 5; point++)
                            {
                                float sparkleRotation = sparkleArmAngle + IM_PI * 0.5f;
                                float outerAngle = (point * 2.0f * IM_PI / 5.0f) + sparkleRotation;
                                float innerAngle1 = ((point + 0.5f) * 2.0f * IM_PI / 5.0f) + sparkleRotation;
                                float innerAngle2 = ((point - 0.5f) * 2.0f * IM_PI / 5.0f) + sparkleRotation;

                                ImVec2 outerPoint = ImVec2(
                                    sparklePos.x + cosf(outerAngle) * sparkleSize,
                                    sparklePos.y + sinf(outerAngle) * sparkleSize
                                );
                                ImVec2 innerPoint1 = ImVec2(
                                    sparklePos.x + cosf(innerAngle1) * sparkleSize * 0.4f,
                                    sparklePos.y + sinf(innerAngle1) * sparkleSize * 0.4f
                                );
                                ImVec2 innerPoint2 = ImVec2(
                                    sparklePos.x + cosf(innerAngle2) * sparkleSize * 0.4f,
                                    sparklePos.y + sinf(innerAngle2) * sparkleSize * 0.4f
                                );

                                ImVec2 triangle[3] = { outerPoint, innerPoint1, innerPoint2 };
                                drawList->AddConvexPolyFilled(triangle, 3, sparkleColor);
                            }
                        }
                    }
                }
            }
        }
    }

    void EffectsManager::RenderCustomCursor(ImDrawList* drawList)
    {
        if (!m_useCustomCursor) return;

        ImVec2 mousePos = ImGui::GetMousePos();
        float baseSize = 16.0f * m_cursorSize;
        ImU32 cursorColor = IM_COL32(
            (int)(m_colors.cursorColor.x * 255),
            (int)(m_colors.cursorColor.y * 255),
            (int)(m_colors.cursorColor.z * 255),
            (int)(m_colors.cursorColor.w * 255)
        );

        if (m_enableCursorTrail)
        {
            for (size_t i = 0; i < m_cursorTrail.size(); i++)
            {
                const auto& point = m_cursorTrail[i];
                float positionFade = 1.0f - (float(i) / float(m_cursorTrail.size()));
                float combinedAlpha = point.alpha * positionFade;
                float trailSize = baseSize * 0.3f * combinedAlpha;
                ImU32 trailColor = (cursorColor & 0x00FFFFFF) | ((ImU32)(combinedAlpha * 150) << 24);

                drawList->AddCircleFilled(point.position, trailSize, trailColor, 16);
            }
        }

        if (m_cursorGlowIntensity > 0.0f)
        {
            for (int i = 1; i <= 4; i++)       
            {
                float glowExpansion = i * 2.0f;      
                float alpha = (m_cursorGlowIntensity * 0.4f) / (i + 1);    
                ImU32 glowColor = (cursorColor & 0x00FFFFFF) | ((ImU32)(alpha * 255) << 24);

                switch (m_cursorStyle)
                {
                case CURSOR_ARROW:
                case CURSOR_STARRY_ARROW:
                {
                    ImVec2 glowPoints[3] = {
                        ImVec2(mousePos.x - glowExpansion, mousePos.y - glowExpansion),     
                        ImVec2(mousePos.x + (baseSize * 0.80f) + glowExpansion, mousePos.y + (baseSize * 0.60f)),      
                        ImVec2(mousePos.x + (baseSize * 0.60f), mousePos.y + (baseSize * 0.80f) + glowExpansion)       
                    };
                    drawList->AddTriangleFilled(glowPoints[0], glowPoints[1], glowPoints[2], glowColor);
                }
                break;

                case CURSOR_HAND:
                case CURSOR_STARRY_HAND:
                {
                    float handSize = baseSize * 0.85f;
                    ImVec2 glowMin = ImVec2(mousePos.x - (handSize * 0.4f) - glowExpansion, mousePos.y - (handSize * 0.3f) - glowExpansion);
                    ImVec2 glowMax = ImVec2(mousePos.x + (handSize * 0.4f) + glowExpansion, mousePos.y + (handSize * 0.5f) + glowExpansion);
                    drawList->AddRectFilled(glowMin, glowMax, glowColor, (handSize * 0.2f) + glowExpansion);
                }
                break;

                case CURSOR_GALAXY:
                case CURSOR_GALAXY_STAR:
                {
                    float galaxyRadius = baseSize * 0.8f;
                    drawList->AddCircleFilled(mousePos, galaxyRadius + glowExpansion, glowColor, 16);
                }
                break;

                default:
                    drawList->AddCircleFilled(mousePos, baseSize + glowExpansion, glowColor, 16);
                    break;
                }
            }
        }

        switch (m_cursorStyle)
        {
        case CURSOR_ARROW:
        {
            ImVec2 points[3] = {
                ImVec2(mousePos.x, mousePos.y),
                ImVec2(mousePos.x + baseSize * 0.80f, mousePos.y + baseSize * 0.60f),
                ImVec2(mousePos.x + baseSize * 0.60f, mousePos.y + baseSize * 0.80f)
            };

            drawList->AddTriangleFilled(points[0], points[1], points[2], cursorColor);
            drawList->AddTriangle(points[0], points[1], points[2], IM_COL32(255, 255, 255, 150), 1.5f);
        }
        break;

        case CURSOR_STARRY_ARROW:
        {
            ImVec2 points[3] = {
                ImVec2(mousePos.x, mousePos.y),
                ImVec2(mousePos.x + baseSize * 0.80f, mousePos.y + baseSize * 0.60f),
                ImVec2(mousePos.x + baseSize * 0.60f, mousePos.y + baseSize * 0.80f)
            };

            for (int i = 0; i < m_cursorSparkleCount; i++)
            {
                float sparkleSize = m_cursorSparkleSize + sin(ImGui::GetTime() * 3.0f + i) * 0.8f;
                float rotation = ImGui::GetTime() * 1.5f + i;
                ImVec2 sparklePos = ImVec2(
                    points[i % 3].x + sin(ImGui::GetTime() * 2.0f + i) * 4.0f,
                    points[i % 3].y + cos(ImGui::GetTime() * 2.0f + i) * 4.0f
                );
                DrawStar(drawList, sparklePos, sparkleSize, rotation, cursorColor);
            }

            drawList->AddTriangleFilled(points[0], points[1], points[2], cursorColor);
            drawList->AddTriangle(points[0], points[1], points[2], IM_COL32(255, 255, 255, 150), 1.5f);
        }
        break;

        case CURSOR_HAND:
        {
            ImVec2 center = mousePos;
            float handSize = baseSize * 0.85f;

            ImVec2 palmMin = ImVec2(center.x - handSize * 0.4f, center.y - handSize * 0.3f);
            ImVec2 palmMax = ImVec2(center.x + handSize * 0.4f, center.y + handSize * 0.5f);
            drawList->AddRectFilled(palmMin, palmMax, cursorColor, handSize * 0.2f);
            drawList->AddRect(palmMin, palmMax, IM_COL32(255, 255, 255, 150), handSize * 0.2f, 0, 1.5f);

            ImVec2 thumbPos = ImVec2(center.x - handSize * 0.5f, center.y);
            drawList->AddCircleFilled(thumbPos, handSize * 0.15f, cursorColor, 8);
            drawList->AddCircle(thumbPos, handSize * 0.15f, IM_COL32(255, 255, 255, 150), 8, 1.0f);

            for (int i = 0; i < 4; i++)
            {
                float fingerOffset = (i - 1.5f) * handSize * 0.2f;
                ImVec2 fingerBase = ImVec2(center.x + fingerOffset, center.y - handSize * 0.3f);
                ImVec2 fingerTip = ImVec2(center.x + fingerOffset, center.y - handSize * 0.7f);

                float lengthMult = (i == 1 || i == 2) ? 1.0f : 0.8f;
                fingerTip.y = fingerBase.y - (handSize * 0.4f * lengthMult);

                float fingerWidth = handSize * 0.08f;
                ImVec2 fingerMin = ImVec2(fingerBase.x - fingerWidth, fingerTip.y);
                ImVec2 fingerMax = ImVec2(fingerBase.x + fingerWidth, fingerBase.y);

                drawList->AddRectFilled(fingerMin, fingerMax, cursorColor, fingerWidth);
                drawList->AddRect(fingerMin, fingerMax, IM_COL32(255, 255, 255, 150), fingerWidth, 0, 1.0f);

                drawList->AddCircleFilled(fingerTip, fingerWidth, cursorColor, 6);
                drawList->AddCircle(fingerTip, fingerWidth, IM_COL32(255, 255, 255, 150), 12, 1.0f);
            }
        }
        break;

        case CURSOR_STARRY_HAND:
        {
            ImVec2 center = mousePos;
            float handSize = baseSize * 0.85f;

            for (int i = 0; i < m_cursorSparkleCount; i++)
            {
                float angle = (i / (float)m_cursorSparkleCount) * 2.0f * 3.14159f + ImGui::GetTime();
                float radius = handSize * 1.2f;
                float sparkleSize = m_cursorSparkleSize + sinf(ImGui::GetTime() * 4.0f + i) * 0.5f;
                float rotation = ImGui::GetTime() * 2.0f + i;

                ImVec2 sparklePos = ImVec2(
                    center.x + cosf(angle) * radius,
                    center.y + sinf(angle) * radius
                );

                DrawStar(drawList, sparklePos, sparkleSize, rotation, cursorColor);
            }

            ImVec2 palmMin = ImVec2(center.x - handSize * 0.4f, center.y - handSize * 0.3f);
            ImVec2 palmMax = ImVec2(center.x + handSize * 0.4f, center.y + handSize * 0.5f);
            drawList->AddRectFilled(palmMin, palmMax, cursorColor, handSize * 0.2f);
            drawList->AddRect(palmMin, palmMax, IM_COL32(255, 255, 255, 150), handSize * 0.2f, 0, 1.5f);

            ImVec2 thumbPos = ImVec2(center.x - handSize * 0.5f, center.y);
            drawList->AddCircleFilled(thumbPos, handSize * 0.15f, cursorColor, 8);
            drawList->AddCircle(thumbPos, handSize * 0.15f, IM_COL32(255, 255, 255, 150), 8, 1.0f);

            for (int i = 0; i < 4; i++)
            {
                float fingerOffset = (i - 1.5f) * handSize * 0.2f;
                ImVec2 fingerBase = ImVec2(center.x + fingerOffset, center.y - handSize * 0.3f);
                ImVec2 fingerTip = ImVec2(center.x + fingerOffset, center.y - handSize * 0.7f);

                float lengthMult = (i == 1 || i == 2) ? 1.0f : 0.8f;
                fingerTip.y = fingerBase.y - (handSize * 0.4f * lengthMult);

                float fingerWidth = handSize * 0.08f;
                ImVec2 fingerMin = ImVec2(fingerBase.x - fingerWidth, fingerTip.y);
                ImVec2 fingerMax = ImVec2(fingerBase.x + fingerWidth, fingerBase.y);

                drawList->AddRectFilled(fingerMin, fingerMax, cursorColor, fingerWidth);
                drawList->AddRect(fingerMin, fingerMax, IM_COL32(255, 255, 255, 150), fingerWidth, 0, 1.0f);

                drawList->AddCircleFilled(fingerTip, fingerWidth, cursorColor, 6);
                drawList->AddCircle(fingerTip, fingerWidth, IM_COL32(255, 255, 255, 150), 12, 1.0f);
            }
        }
        break;

        case CURSOR_GALAXY:
        {
            this->RenderCursorGalaxy(drawList, mousePos, 1.0f);
        }
        break;

        case CURSOR_GALAXY_STAR:
        {
            ImVec2 starPos = mousePos;

            ImU32 starColor = IM_COL32(
                (int)(m_cursorGalaxySparklesColor.x * 255),
                (int)(m_cursorGalaxySparklesColor.y * 255),
                (int)(m_cursorGalaxySparklesColor.z * 255),
                (int)(m_cursorGalaxySparklesColor.w * 255)
            );
            DrawStar(drawList, starPos, 1.0f, ImGui::GetTime() * 2.0f, starColor);

            ImVec2 galaxyCenter = ImVec2(mousePos.x + 15.0f, mousePos.y + 15.0f);
            this->RenderCursorGalaxy(drawList, galaxyCenter, 0.5f);
        }
        break;

        default:
            break;
        }
    }

}