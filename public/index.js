document
	.getElementsByTagName("input")[0]
	.addEventListener("keydown", (event) => {
		if (event.key == "Enter") writeList(event.target.value, "search");
	});

/**
 * writes out books by specified filter/search to html
 **/
function writeList(param, query) {
	param = param.toLowerCase();
	if (param == "return") param = "return_date";
	fetch(`data.csv?${query}=${param}`)
		.then((response) => response.text())
		.then((data) => {
			// console.log(data.toString());
			let rows = data.split("\n");
			let pRows = rows.map((element) => {
				return element.split(",");
			});
			console.log(pRows);
			// clear list
			let items = document.querySelectorAll(".book-item:not(.head)");
			items.forEach((element) => {
				element.remove();
			});

			// add (i know i could simplify this but i wont)
			for (let i = 0; i < pRows.length; i++) {
				// console.log(pRows[i]);
				for (let j = 0; j < pRows[i].length; j++) {
					let container = document.getElementById("book-container");
					let el = document.createElement("div");

					el.classList.add("book-item");

					container.append(el);
					if (j == pRows[i].length - 1) {
						el.classList.add("condition");
						let place = pRows[i][j];
						if (place == " 00-00-0001") place = "in stock";
						el.innerHTML = `
    					<img src="borrowIcon.png" alt="Borrow Icon" />
   						<img src="returnIcon.png" alt="Return Icon" />
    					${place}
   						<img src="remove.png" alt="Remove Icon" />`;
						continue;
					}
					el.innerText = pRows[i][j];
					// if (j == pRows[i].length )
				}
			}
			yuck();
		})
		.catch((error) => {
			console.error("error fetching:", error);
		});
}

function yuck() {
	let rows = document.getElementsByClassName("book-item");
	let grid = document.getElementById("book-container");

	Array.from(rows).forEach((element, i) => {
		element.addEventListener("mouseover", () => {
			let row = Math.floor(i / 5);

			for (let n = 0; n < rows.length; n++) {
				if (Math.floor(n / 5) === row) {
					Array.from(rows[n].getElementsByTagName("img")).forEach(
						(img) => {
							img.style.visibility = "visible";
						}
					);
				} // yuck
			}
		});

		element.addEventListener("mouseout", () => {
			let row = Math.floor(i / 5);

			for (let n = 0; n < rows.length; n++) {
				if (Math.floor(n / 5) === row) {
					Array.from(rows[n].getElementsByTagName("img")).forEach(
						(img) => {
							img.style.visibility = "hidden";
						}
					);
				}
			}
		});
	});
}

writeList("popularity", "sort");
