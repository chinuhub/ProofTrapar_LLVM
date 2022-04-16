; ModuleID = 'ticket.c'
source_filename = "ticket.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@next_ticket = dso_local global i32 0, align 4
@now_serving = dso_local global i32 0, align 4
@c = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @fetch_and_increment__next_ticket() #0 {
  %1 = alloca i32, align 4
  call void (...) @__VERIFIER_atomic_begin()
  %2 = load i32, i32* @next_ticket, align 4
  %3 = add nsw i32 %2, 1
  %4 = srem i32 %3, 3
  %5 = load i32, i32* @now_serving, align 4
  %6 = icmp eq i32 %4, %5
  br i1 %6, label %7, label %8

7:                                                ; preds = %0
  store i32 3, i32* %1, align 4
  br label %13

8:                                                ; preds = %0
  %9 = load i32, i32* @next_ticket, align 4
  store i32 %9, i32* %1, align 4
  %10 = load i32, i32* @next_ticket, align 4
  %11 = add nsw i32 %10, 1
  %12 = srem i32 %11, 3
  store i32 %12, i32* @next_ticket, align 4
  br label %13

13:                                               ; preds = %8, %7
  call void (...) @__VERIFIER_atomic_end()
  %14 = load i32, i32* %1, align 4
  ret i32 %14
}

declare dso_local void @__VERIFIER_atomic_begin(...) #1

declare dso_local void @__VERIFIER_atomic_end(...) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @acquire_lock() #0 {
  %1 = alloca i32, align 4
  %2 = call i32 @fetch_and_increment__next_ticket()
  store i32 %2, i32* %1, align 4
  %3 = load i32, i32* %1, align 4
  %4 = icmp eq i32 %3, 3
  br i1 %4, label %5, label %6

5:                                                ; preds = %0
  br label %14

6:                                                ; preds = %0
  br label %7

7:                                                ; preds = %6, %12
  %8 = load i32, i32* @now_serving, align 4
  %9 = load i32, i32* %1, align 4
  %10 = icmp eq i32 %8, %9
  br i1 %10, label %11, label %12

11:                                               ; preds = %7
  br label %13

12:                                               ; preds = %7
  br label %7

13:                                               ; preds = %11
  br label %14

14:                                               ; preds = %13, %5
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @release_lock() #0 {
  %1 = load i32, i32* @now_serving, align 4
  %2 = add nsw i32 %1, 1
  store i32 %2, i32* @now_serving, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  call void @acquire_lock()
  %1 = load i32, i32* @c, align 4
  %2 = add nsw i32 %1, 1
  store i32 %2, i32* @c, align 4
  %3 = load i32, i32* @c, align 4
  %4 = icmp eq i32 %3, 1
  br i1 %4, label %6, label %5

5:                                                ; preds = %0
  call void @__ASSERT()
  br label %6

6:                                                ; preds = %5, %0
  %7 = load i32, i32* @c, align 4
  %8 = add nsw i32 %7, -1
  store i32 %8, i32* @c, align 4
  call void @release_lock()
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  call void @acquire_lock()
  %1 = load i32, i32* @c, align 4
  %2 = add nsw i32 %1, 1
  store i32 %2, i32* @c, align 4
  %3 = load i32, i32* @c, align 4
  %4 = icmp eq i32 %3, 1
  br i1 %4, label %6, label %5

5:                                                ; preds = %0
  call void @__ASSERT()
  br label %6

6:                                                ; preds = %5, %0
  %7 = load i32, i32* @c, align 4
  %8 = add nsw i32 %7, -1
  store i32 %8, i32* @c, align 4
  call void @release_lock()
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
