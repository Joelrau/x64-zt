glaze = {
	source = path.join(dependencies.basePath, "glaze"),
}

function glaze.import()
	glaze.includes()
end

function glaze.includes()
	includedirs {
		path.join(glaze.source, "include")
	}
end

function glaze.project()

end

table.insert(dependencies, glaze)
