#define _USE_MATH_DEFINES

#include "ParticleSystem.hpp"

#include "Utils.hpp"

#include <xstddef>

ParticleSystem::ParticleSystem() :
	m_particle_size(32.0f, 32.0f), // Default size is 32x32 pixels
	m_velocity(0.0f),
	m_exponential_growth(1.0f, 1.0f),
	m_lifetime_max(0.0f),
	m_rate(0.0f),
	m_timer(0.0f),
	m_is_emitted(false),
	m_is_attenuated(false)
{
}

void ParticleSystem::setTexture(const sf::Texture* texture)
{
	m_instance.setTexture(*texture);
	setParticleSize(sf::Vector2f(texture->getSize()));
	m_instance.setOrigin(m_particle_size * 0.5f);
}

void ParticleSystem::setColor(const sf::Color& color)
{
	m_instance.setColor(color);
}

void ParticleSystem::setParticleSize(const sf::Vector2f& size)
{
	m_particle_size = size;

	setSize(m_instance, size);
	m_instance.setOrigin(size * 0.5f);
}

void ParticleSystem::setEmitter(const sf::Vector2f& emitter)
{
	m_emitter = emitter;
}

void ParticleSystem::setDirection(sf::Angle direction)
{
	m_direction = direction;
}

void ParticleSystem::setDispersion(sf::Angle dispersion)
{
	m_dispersion = dispersion;
}

void ParticleSystem::setVelocity(float velocity)
{
	m_velocity = fabs(velocity);
}

void ParticleSystem::setRespawnRate(float rate)
{
	m_rate = fabs(rate);
}

void ParticleSystem::setRespawnArea(const sf::Vector2f& area)
{
	m_respawn_area = area;
}

void ParticleSystem::setLifeTime(float lifetime)
{
	m_lifetime_max = fabs(lifetime);
}

void ParticleSystem::setExponentialGrowth(const sf::Vector2f& factors)
{
	m_exponential_growth = factors;
}

void ParticleSystem::setEmitted(bool emitted)
{
	m_is_emitted = emitted;
}

void ParticleSystem::setAttenuated(bool attenuation)
{
	m_is_attenuated = attenuation;
}

void ParticleSystem::setExplosion(std::size_t splash_amount, float radius)
{
	if (m_particles.empty())
	{
		setEmitted(false);

		float offset = M_PI * 2 / splash_amount;

		for (size_t i = 0; i < splash_amount; ++i)
		{
			auto particle = std::make_unique<Particle>();
			memcpy(&particle->sprite, &m_instance, sizeof(sf::Sprite));

			float dir = i * offset;
			float sine = std::sin(dir);
			float cosine = std::cos(dir);

			float x = cosine * radius + m_emitter.x;
			float y = sine * radius + m_emitter.y;

			particle->sprite.setPosition(sf::Vector2f(x, y));

			particle->controller.velocity.x = cosine * m_velocity;
			particle->controller.velocity.y = sine * m_velocity;
			particle->controller.lifetime = frand(0, m_lifetime_max) + 1.0f;			

			m_particles.push_back(std::move(particle));
		}
	}	
}

void ParticleSystem::update(float dt)
{
	if (m_is_emitted)
		m_timer += m_rate * dt;

	while (m_timer > 1.0f)
	{
		m_timer -= 1.0f;
		createParticle();
	}

	for (auto p = m_particles.begin(); p != m_particles.end();)
	{
		auto particle = (*p).get();		

		if (particle->controller.lifetime > 0.0f)
		{
			particle->sprite.move(particle->controller.velocity * dt);

			float ratio = particle->controller.lifetime / m_lifetime_max;

			if (m_is_attenuated)
			{
				sf::Color color = particle->sprite.getColor();
				color.a = static_cast<sf::Uint8>(ratio * 255);
				particle->sprite.setColor(color);
			}		

			particle->sprite.scale(m_exponential_growth);		

			p++;
		}
		else
			p = m_particles.erase(p);

		particle->controller.lifetime -= dt;
	}
}

// Getters

const sf::Texture* ParticleSystem::getTexture() const
{
	return m_instance.getTexture();
}

const sf::Color& ParticleSystem::getColor() const
{
	return m_instance.getColor();
}

const sf::Vector2f& ParticleSystem::getParticleSize() const
{
	return m_particle_size;
}

const sf::Vector2f& ParticleSystem::getEmitter() const
{
	return m_emitter;
}

sf::Angle ParticleSystem::getDirection() const
{
	return m_direction;
}

sf::Angle ParticleSystem::getDispersion() const
{
	return m_dispersion;
}

float ParticleSystem::getVelocity() const
{
	return m_velocity;
}

float ParticleSystem::getRespawnRate() const
{
	return m_rate;
}

const sf::Vector2f& ParticleSystem::getRespawnArea() const
{
	return m_respawn_area;
}

float ParticleSystem::getLifeTime() const
{
	return m_lifetime_max;
}

const sf::Vector2f& ParticleSystem::getExponentialGrowth() const
{
	return m_exponential_growth;
}

bool ParticleSystem::isEmitted() const
{
	return m_is_emitted;
}

bool ParticleSystem::isAttenuated() const
{
	return m_is_attenuated;
}

void ParticleSystem::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{
	for (auto& particle : m_particles)
		target.draw(particle->sprite);
}

void ParticleSystem::createParticle()
{
	auto particle = std::make_unique<Particle>();
	memcpy(&particle->sprite, &m_instance, sizeof(sf::Sprite));

	float half_disp = (m_dispersion * 0.5f).asDegrees();
	float random = frand(-half_disp, half_disp);
	float angle = (m_direction + sf::degrees(random)).asRadians();

	particle->controller.velocity.x = std::cos(angle) * m_velocity;
	particle->controller.velocity.y = std::sin(angle) * m_velocity;

	particle->controller.lifetime = frand(0, m_lifetime_max) + 1.0f;

	sf::Vector2f respawn_point = rand2f(m_respawn_area);
	sf::Vector2f offset = m_emitter + respawn_point;

	particle->sprite.setPosition(offset);
	particle->sprite.setRotation(sf::degrees(frand(0.0f, 360.0f)));

	m_particles.push_back(std::move(particle));
}

void ParticleSystem::setSize(sf::Sprite& sprite, const sf::Vector2f& size)
{
	if (auto& rect = sprite.getTextureRect(); rect.width && rect.height)
	{
		float width = size.x / std::abs(rect.width);
		float height = size.y / std::abs(rect.height);

		sprite.setScale(sf::Vector2f(width, height));
	}
}
