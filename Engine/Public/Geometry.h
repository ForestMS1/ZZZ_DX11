#pragma once

NS_BEGIN(Engine)

template<typename T>
class ENGINE_DLL Geometry final
{
public:
	Geometry() = default;
	~Geometry() = default;

	//---------------------------------------------------------Getter------------------------------------------------------------
	uint32 GetVertexCount() const { return static_cast<uint32>(_vertices.size()); }
	void* GetVertexData() { return _vertices.data(); }
	const vector<T>& GetVertices() { return _vertices; }

	uint32 GetIndexCount() const { return static_cast<uint32>(_indices.size()); }
	void* GetIndexData() { return _indices.data(); }
	const vector<uint32> GetIndices() { return _indices; }
	//---------------------------------------------------------Getter------------------------------------------------------------


	//---------------------------------------------------------Setter------------------------------------------------------------
	void AddVertex(const T& vertex) { _vertices.push_back(vertex); }
	void AddVertices(const vector<T>& vertices) { _vertices.insert(_vertices.end(), vertices.begin(), vertices.end()); }
	void SetVertices(const vector<T>& vertices) { _vertices = vertices; }

	void AddIndex(uint32 index) { _indices.push_back(index); }
	void AddIndices(const vector<uint32>& indices) { _indices.insert(_indices.end(), indices.begin(), indices.end()); }
	void SetIndices(const vector<uint32>& indices) { _indices = indices; }
	//---------------------------------------------------------Setter------------------------------------------------------------


private:
	vector<T> _vertices;
	vector<uint32> _indices;

};

NS_END
