<template>
  <header class="header">
    <div class="header__content">
      <nav>
        <ul class="header__links-list">
          <li>
            <router-link to="/">Хосты</router-link>
          </li>
          <li>
            <router-link to="/controllers">Контроллеры</router-link>
          </li>
        </ul>
      </nav>
    </div>
  </header>
</template>

<script setup lang="ts">

</script>

<style scoped>
.header {
  position: sticky;
  top: 1em;
  z-index: 1000;
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: 50px;
  padding: 1em 1rem;
  background-color: #333;
  border-radius: 4px;
}
.header__links-list {
  display: flex;
  gap: 1em;
  list-style: none;
  padding: 0;
}
.header__links-list a {
  text-decoration: none;
  color: white;
  font-size: 1rem;
}
</style>
