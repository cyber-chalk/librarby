function writeList(param) {
	fetch(`data.csv?sort=${param}`)
		.then((response) => response.text())
		.then((data) => {
			rows = data.split("\n");
			values = rows.split(" "); // might not split since 2d
			// clear list
			items = document.querySelectorAll(".book-item:not(.head)");
			items.forEach((element) => {
				element.remove();
			});
			// add (i know i could simplify this but i wont)
			for (let i = 0; i < rows.length; i++) {
				for (let j = 0; j < values.length; j++) {
					let container = document.getElementById("book-container");
					let el = document.createElement("div");

					el.classList.add("book-item");

					container.append(el);
					if (j == values.length - 1) {
						el.classList.add("condition");
						document.createElement("img").src = "borrowIcon.png";
						el.innerText = values[j];
						document.createElement("img").src = "remove.png";
					}
					el.innerText = values[j];
				}
			}
		})
		.catch((error) => {
			console.error("error fetching:", error);
		});
}

writeList("popularity");

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
