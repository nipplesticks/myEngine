#include "Transform.h"

Transform::Transform()
{
	using namespace DirectX;

	const XMMATRIX scale = XMMatrixScalingFromVector(XMLoadFloat3(&this->m_scale));
	const XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&this->m_rotation));
	const XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&this->m_position));

	XMStoreFloat4x4A(&this->m_worldMatrix, scale * rotation * translation);
}

Transform::~Transform()
{
}

void Transform::Update()
{
	using namespace DirectX;

	const XMMATRIX scale = XMMatrixScalingFromVector(XMLoadFloat3(&this->m_scale));
	const XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&this->m_rotation));
	const XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&this->m_position));

	XMStoreFloat4x4A(&this->m_worldMatrix, scale * rotation * translation);
}

void Transform::SetPosition(const DirectX::XMFLOAT3 & position)
{
	this->m_position = position;
}

void Transform::SetPosition(float x, float y, float z)
{
	this->SetPosition(DirectX::XMFLOAT3(x, y, z));
}

void Transform::Translate(const DirectX::XMFLOAT3 & translation)
{
	this->SetPosition(m_position.x + translation.x,
		m_position.y + translation.y,
		m_position.z + translation.z);
}

void Transform::Translate(float x, float y, float z)
{
	this->SetPosition(m_position.x + x,
		m_position.y + y,
		m_position.z + z);
}

void Transform::SetRotation(const DirectX::XMFLOAT3 & rotation)
{
	this->m_rotation = rotation;
}

void Transform::SetRotation(float x, float y, float z)
{
	this->SetRotation(DirectX::XMFLOAT3(x, y, z));
}

void Transform::Rotate(const DirectX::XMFLOAT3 & rotation)
{
	this->SetRotation(m_rotation.x + rotation.x,
		m_rotation.y + rotation.y,
		m_rotation.z + rotation.z);
}

void Transform::Rotate(float x, float y, float z)
{
	this->SetRotation(m_rotation.x + x,
		m_rotation.y + y,
		m_rotation.z + z);
}

void Transform::SetScale(const DirectX::XMFLOAT3 & scale)
{
	this->m_scale = scale;
}

void Transform::SetScale(float x, float y, float z)
{
	this->SetScale(DirectX::XMFLOAT3(x, y, z));
}

void Transform::Scale(const DirectX::XMFLOAT3 & scale)
{
	this->SetScale(m_scale.x + scale.x,
		m_scale.y + scale.y,
		m_scale.z + scale.z);
}

void Transform::Scale(float x, float y, float z)
{
	this->SetScale(m_scale.x + x,
		m_scale.y + y,
		m_scale.z + z);
}

const DirectX::XMFLOAT3 & Transform::GetPosition() const
{
	return m_position;
}

const DirectX::XMFLOAT3 & Transform::GetRotation() const
{
	return m_rotation;
}

const DirectX::XMFLOAT3 & Transform::GetScale() const
{
	return m_scale;
}

const DirectX::XMFLOAT4X4A & Transform::GetWorldMatrix() const
{
	return m_worldMatrix;
}
